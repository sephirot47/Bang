#ifndef EDITORRECTTRANSFORMGIZMO_H
#define EDITORRECTTRANSFORMGIZMO_H

#include "Bang/EditorGizmo.h"

class EditorRectTransformAnchorGizmo;
class EditorRectTransformCornerGizmo;
class EditorRectTransformGizmo : public EditorGizmo
{
public:
    EditorRectTransformGizmo(GameObject *attachedGameObject);
    virtual ~EditorRectTransformGizmo();

    void OnEditorUpdate() override;
    void OnDrawGizmos(bool depthed, bool overlay) override;

    bool SomeOtherCornerGrabbed(EditorRectTransformCornerGizmo *corner) const;

private:
    EditorRectTransformCornerGizmo *m_leftTopCorner  = nullptr;
    EditorRectTransformCornerGizmo *m_rightTopCorner = nullptr;
    EditorRectTransformCornerGizmo *m_leftBotCorner  = nullptr;
    EditorRectTransformCornerGizmo *m_rightBotCorner = nullptr;
    EditorRectTransformCornerGizmo *m_centerCorner   = nullptr;

    EditorRectTransformAnchorGizmo *m_leftTopAnchor  = nullptr;
    EditorRectTransformAnchorGizmo *m_rightTopAnchor = nullptr;
    EditorRectTransformAnchorGizmo *m_leftBotAnchor  = nullptr;
    EditorRectTransformAnchorGizmo *m_rightBotAnchor = nullptr;
};

#endif // EDITORRECTTRANSFORMGIZMO_H
