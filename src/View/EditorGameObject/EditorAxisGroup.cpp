#include "Bang/EditorAxisGroup.h"

#include "Bang/Scene.h"
#include "Bang/Camera.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/EditorState.h"
#include "Bang/SceneManager.h"
#include "Bang/EditorScaleAxisGroup.h"
#include "Bang/EditorRotateAxisGroup.h"
#include "Bang/EditorTranslateAxisGroup.h"

EditorAxisGroup::EditorAxisGroup(GameObject *attachedGameObject) : GameObject("EditorAxisGroup")
{
    m_attachedGameObject = attachedGameObject;
    m_etag = new EditorTranslateAxisGroup(attachedGameObject);
    m_erag = new EditorRotateAxisGroup(attachedGameObject);
    m_esag = new EditorScaleAxisGroup(attachedGameObject);

    m_etag->SetParent(this);
    m_erag->SetParent(this);
    m_esag->SetParent(this);

    DisableAllGroups();

    AddHideFlag(HideFlags::HideAndDontSave);
}

void EditorAxisGroup::DisableAllGroups()
{
    m_etag->SetEnabled(false);
    m_erag->SetEnabled(false);
    m_esag->SetEnabled(false);
}

void EditorAxisGroup::OnEditorUpdate()
{
    EditorState::TransformMode tm =
            EditorState::GetCurrentTransformMode();

    DisableAllGroups();
    if (tm == EditorState::TransformMode::Translate)
    {
        m_etag->SetEnabled(true);
    }
    else if (tm == EditorState::TransformMode::Rotate)
    {
        m_erag->SetEnabled(true);
    }
    else if (tm == EditorState::TransformMode::Scale)
    {
        m_esag->SetEnabled(true);
    }

    //Attached GameObject positioning
    if (m_attachedGameObject)
    {
        Transform *at = m_attachedGameObject->transform;
        if (at)
        {
            transform->SetPosition(at->GetPosition()); // Pivot
            //Box bbox = m_attachedGameObject->GetBoundingBox();
            //transform->SetPosition( bbox.GetCenter() ); // Center

            if (EditorState::IsUsingGlobalCoords())
            {
                transform->SetLocalRotation( Quaternion() );
            }
            else
            {
                transform->SetLocalRotation( at->GetRotation() );
            }

            Camera *cam = SceneManager::GetActiveScene()->GetCamera();
            Vector3 camPos = cam->gameObject->transform->GetPosition();
            Vector3 attPos = at->GetPosition();
            float distanceToCamera = Vector3::Distance(camPos, attPos);

            transform->SetScale(distanceToCamera * c_sizeBoost);
        }
    }
}


