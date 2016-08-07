#include "InspectorWidget.h"
#include "WindowEventManager.h"
#include "Component.h"

#include "AttributeWidget.h"
#include "AttrWidgetEnum.h"
#include "AttrWidgetFile.h"
#include "AttrWidgetFloat.h"
#include "AttrWidgetString.h"
#include "AttrWidgetVectorFloat.h"
#include "AttrWidgetBool.h"
#include "AttrWidgetColor.h"
#include "AttrWidgetButton.h"

#include "WindowMain.h"
#include "GameObject.h"

InspectorWidget::InspectorWidget() : IDroppableQWidget()
{
}

InspectorWidget::InspectorWidget(const std::string &title,
                                 IInspectable *relatedInspectable) : InspectorWidget()
{
    m_relatedInspectable = relatedInspectable;

    XMLNode xmlInfo = GetInspectableXMLInfo();
    ConstructFromWidgetXMLInfo(title, xmlInfo);

    setAcceptDrops(true);
}

void InspectorWidget::ConstructFromWidgetXMLInfo(
        const std::string &title, XMLNode &xmlInfo, bool autoUpdate)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    m_titleLayout = new QHBoxLayout();

    std::string fTitle = StringUtils::FormatInspectorLabel(title);
    m_titleLabel = new QLabel( QString(fTitle.c_str()) );
    QFont font = m_titleLabel->font();
    font.setBold(true);
    m_titleLabel->setFont(font);

    m_titleLayout->addWidget(m_titleLabel, 10);

    mainLayout->addLayout(m_titleLayout);
    CreateWidgetSlots(xmlInfo);
    RefreshWidgetValues(); // Initial catch of values
    m_created = true;
    show();

    //

    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(OnCustomContextMenuRequested(QPoint)));

    if (autoUpdate)
    {
        m_updateTimer = new QTimer(this); //Every X seconds, update all the slots values
        connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(RefreshWidgetValues()));
        m_updateTimer->start(100);
    }
}

InspectorWidget::~InspectorWidget()
{
    //m_updateTimer->stop();
    delete m_updateTimer;
}

void InspectorWidget::OnDropFromExplorer(const File &f, QDropEvent *e)
{
    Logger_Log(f.GetRelativePath());
}

void InspectorWidget::OnDropFromHierarchy(GameObject *go, QDropEvent *e)
{
}

XMLNode InspectorWidget::GetInspectableXMLInfo() const
{
    XMLNode xmlInfo;
    m_relatedInspectable->OnInspectorXMLNeeded(&xmlInfo);
    return xmlInfo;
}

XMLNode InspectorWidget::GetWidgetXMLInfo() const
{
    // Gather all attributes
    XMLNode xmlInfo;
    xmlInfo.SetTagName(m_tagName);
    for (XMLAttribute attribute : m_attributes)
    {
        std::string attrName = attribute.GetName();
        XMLAttribute::Type attrType = attribute.GetType();

        if (m_attrNameToAttrWidget.find(attrName) != m_attrNameToAttrWidget.end())
        {
            AttributeWidget *aw = m_attrNameToAttrWidget[attrName];
            if (attribute.HasVectoredType())
            {
                AttrWidgetVectorFloat *awv = static_cast<AttrWidgetVectorFloat*>(aw);
                std::vector<float> v = awv->GetValue();
                if (attrType == XMLAttribute::Type::Float)
                {
                    attribute.SetFloat(v[0], attribute.GetProperties());
                }
                else if (attrType == XMLAttribute::Type::Vector2)
                {
                    attribute.SetVector2(Vector2(v[0], v[1]), attribute.GetProperties());
                }
                else if (attrType == XMLAttribute::Type::Vector3)
                {
                    attribute.SetVector3(Vector3(v[0], v[1], v[2]), attribute.GetProperties());
                }
                else if (attrType == XMLAttribute::Type::Vector4 ||
                         attrType == XMLAttribute::Type::Quaternion ||
                         attrType == XMLAttribute::Type::Rect)
                {
                    attribute.SetVector4(Vector4(v[0], v[1], v[2], v[3]), attribute.GetProperties());
                }
            }
            else if (attrType == XMLAttribute::Type::File)
            {
                AttrWidgetFile *awf = static_cast<AttrWidgetFile*>(aw);
                attribute.SetFilepath(awf->GetValue(),
                                      attribute.GetPropertyValue(XMLProperty::FileExtension.GetName()),
                                      attribute.GetProperties());
            }
            else if (attrType == XMLAttribute::Type::String)
            {
                AttrWidgetString *aws = static_cast<AttrWidgetString*>(aw);
                attribute.SetString(aws->GetValue(), attribute.GetProperties());
            }
            else if (attrType == XMLAttribute::Type::Bool)
            {
                AttrWidgetBool *wb = static_cast<AttrWidgetBool*>(aw);
                attribute.SetBool(wb->GetValue(), attribute.GetProperties());
            }
            else if (attrType == XMLAttribute::Type::Enum)
            {
                AttrWidgetEnum *awe = static_cast<AttrWidgetEnum*>(aw);
                attribute.SetEnum(attribute.GetEnumNames(), awe->GetValue(), // selected index
                                  attribute.GetProperties());
            }
            else if (attrType == XMLAttribute::Type::Color)
            {
                AttrWidgetColor *awc = static_cast<AttrWidgetColor*>(aw);
                attribute.SetColor(awc->GetValue(), attribute.GetProperties());
            }
        }
        xmlInfo.SetAttribute(attribute);
    }

    return xmlInfo;
}

void InspectorWidget::SetTitle(const std::string &title)
{
    m_titleLabel->setText(QString::fromStdString(title));
}

void InspectorWidget::OnCustomContextMenuRequested(QPoint point)
{
}

void InspectorWidget::RefreshWidgetValues()
{
    XMLNode xmlInfo = GetInspectableXMLInfo();
    xmlInfo.SetTagName(m_tagName);
    for (auto itAttr : xmlInfo.GetAttributes())
    {
        XMLAttribute attribute = itAttr.second;
        std::string attrName  = attribute.GetName();
        XMLAttribute::Type attrType = attribute.GetType();

        if( m_attrNameToAttrWidget.find(attrName) != m_attrNameToAttrWidget.end())
        {
            AttributeWidget *ws = m_attrNameToAttrWidget[attrName];
            if (attribute.HasVectoredType())
            {
                AttrWidgetVectorFloat *wv = static_cast<AttrWidgetVectorFloat*>(ws);
                if (attrType == XMLAttribute::Type::Float)
                {
                    float v = xmlInfo.GetFloat(attrName);
                    wv->SetValue({v});
                }
                else if (attrType == XMLAttribute::Type::Vector2)
                {
                    Vector2 v = xmlInfo.GetVector2(attrName);
                    wv->SetValue({v.x, v.y});
                }
                else if (attrType == XMLAttribute::Type::Vector3)
                {
                    Vector3 v = xmlInfo.GetVector3(attrName);
                    wv->SetValue({v.x, v.y, v.z});
                }
                else if (attrType == XMLAttribute::Type::Vector4 ||
                         attrType == XMLAttribute::Type::Quaternion ||
                         attrType == XMLAttribute::Type::Rect)
                {
                    Vector4 v = xmlInfo.GetVector4(attrName);
                    wv->SetValue({v.x, v.y, v.z, v.w});
                }
                ws = wv;
            }
            else if (attrType == XMLAttribute::Type::File)
            {
                AttrWidgetFile *wf = static_cast<AttrWidgetFile*>(ws);
                wf->SetValue( xmlInfo.GetFilepath(attrName) );
                ws = wf;
            }
            else if (attrType == XMLAttribute::Type::String)
            {
                AttrWidgetString *wss = static_cast<AttrWidgetString*>(ws);
                wss->SetValue( xmlInfo.GetString(attrName) );
                ws = wss;
            }
            else if (attrType == XMLAttribute::Type::Bool)
            {
                AttrWidgetBool *wb = static_cast<AttrWidgetBool*>(ws);
                wb->SetValue( xmlInfo.GetBool(attrName) );
                ws = wb;
            }
            else if (attrType == XMLAttribute::Type::Enum)
            {
                AttrWidgetEnum *we = static_cast<AttrWidgetEnum*>(ws);
                we->SetValue(attribute.GetEnumSelectedIndex());
                ws = we;
            }
            else if (attrType == XMLAttribute::Type::Color)
            {
                AttrWidgetColor *wc = static_cast<AttrWidgetColor*>(ws);
                wc->SetValue(attribute.GetColor());
                ws = wc;
            }

            bool hidden  =  attribute.HasProperty(XMLProperty::Hidden);
            bool enabled = !attribute.HasProperty(XMLProperty::Disabled);
            ws->setEnabled(enabled);
            ws->setVisible(!hidden);
            ws->setHidden(hidden);
        }
    }
}

void InspectorWidget::CreateWidgetSlots(XMLNode &xmlInfo)
{
    m_tagName = xmlInfo.GetTagName();

    for (auto itAttr : xmlInfo.GetAttributes())
    {
        XMLAttribute attribute = itAttr.second;
        m_attributes.push_back(attribute);
        AttributeWidget *w = AttributeWidget::FromXMLAttribute(attribute, this);
        if (w)
        {
            m_attrNameToAttrWidget[attribute.GetName()] = w;
            layout()->addWidget(w);
            w->show();
        }
    }
}

void InspectorWidget::_OnSlotValueChanged()
{
    NONULL(m_relatedInspectable);
    if (m_created)
    {
        XMLNode xmlInfo = GetWidgetXMLInfo();
        m_relatedInspectable->OnInspectorXMLChanged(&xmlInfo);
    }
    WindowEventManager::GetInstance()->NotifyInspectorSlotChanged(this);
}

void InspectorWidget::_OnSlotValueChanged(int _)
{
    _OnSlotValueChanged();
}
void InspectorWidget::_OnSlotValueChanged(double _)
{
    _OnSlotValueChanged();
}
void InspectorWidget::_OnSlotValueChanged(QString _)
{
    _OnSlotValueChanged();
}
