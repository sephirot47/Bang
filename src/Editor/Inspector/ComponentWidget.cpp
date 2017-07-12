#include "Bang/ComponentWidget.h"

#include "Bang/Chrono.h"
#include "Bang/Component.h"
#include "Bang/IconManager.h"
#include "Bang/EditorWindow.h"

ComponentWidget::ComponentWidget(Component *relatedComponent) :
    InspectorWidget(),
    m_iconLabel(this),
    m_cwContextMenu(this)
{
    p_component = relatedComponent;
}

ComponentWidget::~ComponentWidget()
{
}
void ComponentWidget::OnDestroy()
{
    InspectorWidget::OnDestroy();
}

void ComponentWidget::InitExtra()
{
    InspectorWidget::InitExtra();

    SetIcon(IconManager::GetIcon(p_component->GetClassName()));
    SetTitle(p_component->GetClassName());
    m_closed = p_component->IsClosedInInspector();

    SetClosed(m_closed);
    UpdateCloseOpenButtonIcon();

    if (!p_component->GetClassName().Contains("Transform"))
    {
        m_enabledCheckbox.setChecked(p_component->IsEnabled(false));
        connect(&m_enabledCheckbox, SIGNAL(clicked(bool)),
                this, SLOT(OnEnabledCheckboxPressed(bool)));
        m_headerLayout.addWidget(&m_enabledCheckbox, 0,
                                 Qt::AlignRight | Qt::AlignVCenter);
    }
}

int ComponentWidget::GetHeightSizeHint() const
{
    return InspectorWidget::GetHeightSizeHint();
}

void ComponentWidget::OnEnabledCheckboxPressed(bool checked)
{
    ENSURE(p_component);
    p_component->SetEnabled(checked);
}

void ComponentWidget::SetClosed(bool closed)
{
    ENSURE(p_component);
    InspectorWidget::SetClosed(closed);
    p_component->SetClosedInInspector(closed);
}

