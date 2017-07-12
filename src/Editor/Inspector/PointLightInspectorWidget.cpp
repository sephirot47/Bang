#include "Bang/PointLightInspectorWidget.h"

PointLightInspectorWidget::PointLightInspectorWidget(
        PointLight *pointLight)
    : LightInspectorWidget(pointLight)
{
    p_pointLight = pointLight;
}

PointLightInspectorWidget::~PointLightInspectorWidget()
{
}

void PointLightInspectorWidget::InitExtra()
{
    LightInspectorWidget::InitExtra();

    m_rangeAW   = new AttrWidgetFloat("Range");

    InsertAttributeWidget(m_rangeAW);
}

void PointLightInspectorWidget::Refresh()
{
    LightInspectorWidget::Refresh();

    m_rangeAW->SetValue( p_pointLight->GetRange() );
}

void PointLightInspectorWidget::OnAttrWidgetValueChanged(
        IAttributeWidget *attrWidget)
{
    LightInspectorWidget::OnAttrWidgetValueChanged(attrWidget);

    if (attrWidget == m_rangeAW)
    {
        p_pointLight->SetRange( m_rangeAW->GetValue() );
    }
}

void PointLightInspectorWidget::OnDestroy()
{
    LightInspectorWidget::OnDestroy();
}