#include "EditorRotateAxisGroup.h"

#include "Debug.h"
#include "Gizmos.h"
#include "Camera.h"
#include "Toolbar.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "CircleRenderer.h"
#include "EditorRotateAxis.h"

EditorRotateAxisGroup::EditorRotateAxisGroup(GameObject *attachedGameObject)
    : EditorGameObject("EditorRotateAxisGroup")
{
    m_axisX = new EditorRotateAxis(EditorAxis::EditorAxisDirection::X, attachedGameObject);
    m_axisY = new EditorRotateAxis(EditorAxis::EditorAxisDirection::Y, attachedGameObject);
    m_axisZ = new EditorRotateAxis(EditorAxis::EditorAxisDirection::Z, attachedGameObject);

    m_axisX->SetParent(this);
    m_axisY->SetParent(this);
    m_axisZ->SetParent(this);
}

EditorRotateAxisGroup::~EditorRotateAxisGroup()
{
}

void EditorRotateAxisGroup::OnUpdate()
{
    EditorGameObject::OnUpdate();
}

void EditorRotateAxisGroup::OnDrawGizmosOverlay()
{
    EditorGameObject::OnDrawGizmosOverlay();

    Vector3 center = m_axisX->m_attachedGameObject->transform->GetPosition();
    float radius = 0.95f * m_axisX->m_circle->GetRadius() * transform->GetScale().x;

    // Billboard-scaling taken into account too
    Gizmos::SetColor(Color::White * Color(1, 1, 1, 0.4f));
    Gizmos::SetReceivesLighting(false);
    Gizmos::SetDrawWireframe(false);
    Gizmos::RenderSphere(center, radius);
}
