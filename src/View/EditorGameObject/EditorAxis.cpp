#include "EditorAxis.h"

#include "Input.h"
#include "Camera.h"
#include "Screen.h"
#include "Material.h"
#include "Renderer.h"
#include "Transform.h"
#include "AssetsManager.h"

EditorAxis::EditorAxis(EditorAxis::EditorAxisDirection dir,
                       GameObject *attachedGameObject)
    : EditorGizmo(attachedGameObject)
{
    m_oAxisDirection = EditorAxis::GetVectorFromDir(dir);

    m_material = AssetsManager::Load<Material>("Materials/D2G_Default.bmat",
                                               true);
    m_material = new Material(*m_material);

    m_lineColor = Color(m_oAxisDirection.Abs(), 1);
}

EditorAxis::~EditorAxis()
{
    delete m_material;
}

String EditorAxis::GetStringFromDir(EditorAxis::EditorAxisDirection dir)
{
    if (dir == EditorAxisDirection::X) return "X";
    else if (dir == EditorAxisDirection::Y) return "Y";
    else if (dir == EditorAxisDirection::Z) return "Z";
    else return "Central";
}

Vector3 EditorAxis::GetVectorFromDir(EditorAxis::EditorAxisDirection dir)
{
    if (dir == EditorAxisDirection::X) return Vector3::Right;
    else if (dir == EditorAxisDirection::Y) return Vector3::Up;
    else if (dir == EditorAxisDirection::Z) return Vector3::Forward;
    else return Vector3::One; // Central
}

void EditorAxis::OnUpdate()
{
    EditorGizmo::OnUpdate();

    // Process line color
    bool thickerLine = m_mouseIsOver || m_grabbed;
    float lineWidth = m_axisLineWidth * (thickerLine ? 1.5f : 1.0f);

    Color diffColor;
    if (m_grabbed)
    {
        diffColor = c_colorGrabbed;
    }
    else
    {
        float fading = m_mouseIsOver ? c_colorFadingNotGrabbed :
                                       c_colorFadingNotHover;
        diffColor = Color(m_lineColor * fading, 1);
    }

    GetAxisRenderer()->SetLineWidth(lineWidth);
    m_material->SetDiffuseColor(diffColor);
}
