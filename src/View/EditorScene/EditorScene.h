#ifndef EDITORSCENE_H
#define EDITORSCENE_H

#include "Scene.h"
#include "EditorFloor.h"
#include "EditorCamera.h"
#include "SelectionFramebuffer.h"
#include "EditorDebugGameObject.h"
#include "EditorTranslateAxisGroup.h"

class EditorScene : public Scene
{
private:
    EditorDebugGameObject *m_debugGameObject = nullptr;
    EditorCamera *m_cameraGameObject = nullptr;
    EditorTranslateAxisGroup *m_axises = nullptr;
    EditorFloor *m_floor = nullptr;

    SelectionFramebuffer *m_selectionFramebuffer = nullptr;

public:
    EditorScene();
    virtual ~EditorScene();

    virtual void _OnStart () override;
    virtual void _OnResize (int newWidth, int newHeight) override;
    virtual void _OnRender () override;

    SelectionFramebuffer* GetSelectionFramebuffer() const;

    void DebugDrawLine(const Vector3 &origin,
                       const Vector3 &destiny,
                       const Color &color = Color::green,
                       float lineWidth = 1.0f,
                       float secsTime = 1.0f,
                       bool depthTest = true) override;

    virtual void DebugDrawScreenLine (
                                const Vector2 &origin,
                                const Vector2 &destiny,
                                const Color &color = Color::green,
                                float lineWidth = 1.0f,
                                float secsTime = 1.0f) override;

    bool IsEditorGameObject() const override;
};

#endif // EDITORSCENE_H
