#include "EditorScaleAxis.h"

#include "Toolbar.h"

EditorScaleAxis::EditorScaleAxis(EditorAxis::EditorAxisDirection dir,
                                 GameObject *attachedGameObject)
    : EditorAxis(dir, attachedGameObject)
{
    SetName("EditorScaleAxisGroup" + EditorAxis::GetStringFromDir(dir));

    m_line = AddComponent<SingleLineRenderer>();
    m_line->SetDestiny(m_oAxisDirection);
    m_line->SetMaterial(m_material);
    m_line->SetLineWidth(2.0f);
    m_line->SetReceivesLighting(false);

    m_line->SetActivateGLStatesBeforeRenderingForSelectionFunction([]()
        {
            glLineWidth(25.0f); // Easier grab
        }
    );

    m_axisCap = MeshFactory::GetCubeEditorGameObject();
    m_axisCap->SetParent(this);
    m_axisCap->transform->SetLocalPosition(m_oAxisDirection);
    m_axisCap->transform->SetLocalScale(Vector3(0.2f));
    m_axisCap->GetComponent<MeshRenderer>()->SetMaterial(m_material);
    m_axisCap->GetComponent<MeshRenderer>()->SetReceivesLighting(true);

    m_line->SetEnabled(false);
    m_axisCap->GetComponent<Renderer>()->SetEnabled(false);
}

EditorScaleAxis::~EditorScaleAxis()
{

}


void EditorScaleAxis::OnUpdate()
{
    EditorAxis::OnUpdate();

    Camera *cam = SceneManager::GetActiveScene()->GetCamera(); NONULL(cam);
    Transform *camTransform = cam->gameObject->transform; NONULL(camTransform);
    GameObject *ago = m_attachedGameObject; NONULL(ago->transform);
    Vector3 wCamPos = camTransform->GetPosition();

    SetEnabled(Toolbar::GetInstance()->GetSelectedTransformMode() ==
               Toolbar::TransformMode::Scale);
    if (m_grabbed)
    {
        Vector2 sMouseDelta = Input::GetMouseDelta() * Vector2(1.0f, -1.0f);
        if (sMouseDelta.Length() > 0.0f)
        {
            Vector3 wAxisDir, parentAxisDir;
            if (Toolbar::GetInstance()->IsInGlobalCoordsMode())
            {
                wAxisDir = m_oAxisDirection;
                parentAxisDir = ago->transform->WorldToLocalDirection(m_oAxisDirection);
            }
            else
            {
                wAxisDir = ago->transform->LocalToWorldDirection(m_oAxisDirection);
                parentAxisDir = m_oAxisDirection;
            }
            wAxisDir.Normalize();
            parentAxisDir.Normalize();

            // Alignment
            Vector3 wAxisCenter = transform->GetPosition();
            Vector2 screenAxisDir = cam->WorldToScreenNDCPoint(wAxisCenter + wAxisDir) -
                                      cam->WorldToScreenNDCPoint(wAxisCenter);
            screenAxisDir.Normalize();
            float alignment = Vector2::Dot(screenAxisDir, sMouseDelta.Normalized());
            //

            Vector3 scaling = alignment *
                              parentAxisDir.Abs() *
                              sMouseDelta.Length() *
                              Vector3::Distance(wCamPos, ago->transform->GetPosition()) * 0.002f;

            scaling *= 1.0f / ago->parent->transform->GetScale();
            //TODO: solve problem with negative scaling and depth :/
            ago->transform->SetLocalScale(ago->transform->GetLocalScale() + scaling);
        }
    }
}

void EditorScaleAxis::OnDrawGizmosNoDepth()
{
    EditorAxis::OnDrawGizmosNoDepth();

    m_line->SetEnabled(true);
    m_axisCap->GetComponent<Renderer>()->SetEnabled(true);

    Gizmos::Render(m_line);
    Gizmos::Render(m_axisCap->GetComponent<Renderer>());

    m_line->SetEnabled(false);
    m_axisCap->GetComponent<Renderer>()->SetEnabled(false);
}

Renderer *EditorScaleAxis::GetAxisRenderer() const
{
    return m_line;
}

