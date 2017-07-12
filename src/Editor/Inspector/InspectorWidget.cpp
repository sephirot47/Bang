#include "Bang/InspectorWidget.h"

#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Bang/WinUndef.h"

#include "Bang/Debug.h"
#include "Bang/Inspector.h"
#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/IInspectable.h"
#include "Bang/EditorWindow.h"
#include "Bang/AttributeWidget.h"
#include "Bang/WindowEventManager.h"
#include "Bang/SerializableObject.h"

InspectorWidget::InspectorWidget() : DragDropQWidget(nullptr)
{
}

void InspectorWidget::Init(IInspectable *inspectable)
{
    p_inspectable = inspectable;

    XMLNode xmlInfo = GetInspectableXMLInfo();
    ConstructFromWidgetXMLInfo(xmlInfo);

    SetIcon( inspectable->GetIcon() );

    setAcceptDrops(true);
    Refresh();
}

void InspectorWidget::ConstructFromWidgetXMLInfo(const XMLNode &xmlInfo)
{
    setVisible(false);

    m_vLayout.addLayout(&m_headerLayout, 0);
    m_vLayout.addLayout(&m_gridLayout, 99);

    m_headerLayout.setSpacing(5);
    m_headerLayout.addWidget(&m_closeOpenButton, 0,
                             Qt::AlignLeft | Qt::AlignVCenter);
    m_headerLayout.addWidget(&m_titleLabel,     99,
                             Qt::AlignLeft | Qt::AlignVCenter);
    m_gridLayout.setSpacing(0);

    m_closeOpenButton.setStyleSheet("padding:0px;border:0px;margin-left:-5px;");
    QObject::connect(&m_closeOpenButton, SIGNAL(clicked()),
                     this, SLOT(OnCloseOpenButtonClicked()));
    UpdateCloseOpenButtonIcon();

    String title = Inspector::FormatInspectorLabel( xmlInfo.GetTagName() );
    m_titleLabel.setText(title.ToQString());
    QFont font = m_titleLabel.font();
    font.setPixelSize(13);
    font.setBold(true);
    m_titleLabel.setFont(font);
    m_titleLabel.setAlignment(Qt::AlignLeft);

    CreateAttributeWidgets(xmlInfo);

    QObject::connect(&m_refreshTimer, SIGNAL(timeout()),
                     this, SLOT(Refresh()));
    m_refreshTimer.start(10);

    setLayout(&m_vLayout);
    m_created = true;
    setVisible(true);
}

InspectorWidget::~InspectorWidget()
{
    OnDestroy();
}

void InspectorWidget::OnDestroy()
{
    m_refreshTimer.stop();
    p_inspectable = nullptr;
    QObject::disconnect(&m_refreshTimer, SIGNAL(timeout()),
                        this, SLOT(Refresh()));
}


XMLNode InspectorWidget::GetInspectableXMLInfo() const
{
    XMLNode xmlInfo;
    if (p_inspectable)
    {
        p_inspectable->GetSerializableObject()->Write(&xmlInfo);
    }
    return xmlInfo;
}

XMLNode InspectorWidget::GetWidgetXMLInfo() const
{
    // Gather all attributes XML info's, mantaining their original XMLProp's.
    XMLNode xmlInfo(m_tagName);
    for (const auto &attrWidget_originalAttr : m_attrWidget_To_XMLAttr)
    {
        const AttributeWidget *aw = attrWidget_originalAttr.first;
        const XMLAttribute &originalAttr = attrWidget_originalAttr.second;
        XMLAttribute newAttr = aw->GetXMLAttribute();
        newAttr.SetName(originalAttr.GetName());
        newAttr.SetProperties(originalAttr.GetProperties());
        xmlInfo.SetAttribute(newAttr);
    }

    return xmlInfo;
}

IInspectable *InspectorWidget::GetInspectable() const
{
    return p_inspectable;
}

QGridLayout *InspectorWidget::GetGridLayout()
{
    return &m_gridLayout;
}

void InspectorWidget::SetTitle(const String &title)
{
    m_titleLabel.setText(title.ToQString());
}

bool InspectorWidget::IsClosed() const
{
    return m_closed;
}

int InspectorWidget::GetHeightSizeHint() const
{
    int heightSizeHint = 0;
    heightSizeHint += 50; // Header height

    // Add up children widget's height size hints
    for (AttributeWidget *aw : m_attributeWidgets)
    {
        heightSizeHint += aw->IsVisible() ? aw->GetHeightSizeHint() : 0;
    }

    return heightSizeHint;
}

void InspectorWidget::Refresh()
{
    ENSURE(p_inspectable);

    XMLNode xmlInfo = GetInspectableXMLInfo();
    xmlInfo.SetTagName(m_tagName);
    for (auto itAttr : xmlInfo.GetAttributesListInOrder())
    {
        const XMLAttribute& attribute = itAttr.second;
        const String& attrName = attribute.GetName();
        if( m_attrName_To_AttrWidget.ContainsKey(attrName))
        {
            AttributeWidget *attrWidget = m_attrName_To_AttrWidget[attrName];
            attrWidget->SetVisible( !IsClosed() &&
                                    !attribute.HasProperty(XMLProperty::Hidden));
            attrWidget->Refresh(attribute);
        }
    }
    UpdateContentMargins();
}

void InspectorWidget::SetIcon(const QPixmap &icon)
{
    QPixmap pm = icon.scaled(16, 16, Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation);
    m_iconLabel.setPixmap(pm);
    m_headerLayout.insertWidget(1, &m_iconLabel, 0, Qt::AlignCenter);
}

void InspectorWidget::CreateAttributeWidgets(const XMLNode &xmlInfo)
{
    m_tagName = xmlInfo.GetTagName();
    for (auto itAttr : xmlInfo.GetAttributesListInOrder())
    {
        const XMLAttribute &attribute = itAttr.second;
        AttributeWidget *aw = AttributeWidget::FromXMLAttribute(attribute);
        if (!aw) { continue; }

        InsertAttributeWidget(aw);
        m_attrWidget_To_XMLAttr.Set(aw, attribute);
        m_attrName_To_AttrWidget.Set(attribute.GetName(), aw);
    }
}

void InspectorWidget::OnAttrWidgetValueChanged()
{
    ENSURE(p_inspectable);
    if (m_created)
    {
        p_inspectable->GetSerializableObject()->Read( GetWidgetXMLInfo() );
    }
    WindowEventManager::GetInstance()->NotifyInspectorSlotChanged(this);
}

void InspectorWidget::OnCloseOpenButtonClicked()
{
    m_closed = !m_closed;
    SetClosed(m_closed);
    Refresh();
    UpdateContentMargins();
    Inspector::GetInstance()->RefreshSizeHints();
}

void InspectorWidget::SetClosed(bool closedWidget)
{
    for (int i = 0; i < m_gridLayout.count(); ++i)
    {
        QLayoutItem *item = m_gridLayout.itemAt(i);
        if (!item->widget()) { continue; }
        item->widget()->setHidden(closedWidget);
    }
    UpdateCloseOpenButtonIcon();
}

void InspectorWidget::InsertAttributeWidget(AttributeWidget *attrWidget,
                                            int _index)
{
    const int index = (_index >= 0 ? _index : m_gridLayout.rowCount());
    m_gridLayout.addWidget(attrWidget, index, 0);
    QObject::connect(attrWidget, SIGNAL(OnValueChanged()),
                     this, SLOT(OnAttrWidgetValueChanged()));

    m_attributeWidgets.PushBack(attrWidget);
}

void InspectorWidget::UpdateContentMargins()
{
    Inspector *insp = Inspector::GetInstance();
    bool scrollbarVisible = insp->verticalScrollBar()->isVisible();
    const int c_marginLeft  = 10;
    const int c_marginTop = IsClosed() ? 0 : 5;
    const int c_marginRight = scrollbarVisible ? 20 : 5;
    const int c_marginBot = IsClosed() ? 0 : 15;
    m_vLayout.setContentsMargins(c_marginLeft,  c_marginTop,
                                  c_marginRight, c_marginBot);
}

void InspectorWidget::UpdateCloseOpenButtonIcon()
{
    static QPixmap closeButtonPixmap(":/qss_icons/rc/branch_closed.png");
    static QPixmap openButtonPixmap(":/qss_icons/rc/branch_open.png");

    const QPixmap &pixmap = m_closed ? closeButtonPixmap : openButtonPixmap;
    m_closeOpenButton.setIcon( QIcon(pixmap) );
}
