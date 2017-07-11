#ifndef EDITORGIZMO_H
#define EDITORGIZMO_H

#include "Bang/Cursor.h"
#include "Bang/GameObject.h"

class EditorGizmo : public GameObject
{
public:
    EditorGizmo(GameObject *attachedGameObject);
    virtual ~EditorGizmo();

    virtual void OnEditorUpdate() override;
    virtual void OnMouseEnter(bool fromChildren) override;
    virtual void OnMouseExit(bool fromChildren) override;

    bool IsGrabbed() const;

protected:
    Cursor::CursorIcon m_cursorIconWhenOver    = Cursor::CursorIcon::Arrow;
    Cursor::CursorIcon m_cursorIconWhenGrabbed = Cursor::CursorIcon::Arrow;

    bool m_grabbed = false;
    bool m_mouseIsOver = false;
    GameObject *m_attachedGameObject = nullptr;
};

#endif // EDITORGIZMO_H