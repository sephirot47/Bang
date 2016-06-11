#ifndef SELECTIONFRAMEBUFFER_H
#define SELECTIONFRAMEBUFFER_H

#include <map>

#include "Scene.h"
#include "Framebuffer.h"
#include "GameObject.h"

/**
 * @brief This is the buffer where each GameObject is drawn into with a unique colour.
 */
class SelectionFramebuffer : public Framebuffer
                             ,public IWindowEventManagerListener
{
private:
    ShaderProgram *program = nullptr;

    long idCount = 0;
    std::map<GameObject*, long> gameObjectToId;
    std::map<long, GameObject*> idToGameObject;

    static Vector3 MapIdToColor(long id);
    static long MapColorToId(const Vector3 &charColor);

    /**
     * @brief Keeps track of the last GameObject the mouse was over.
     * This lets us inform of mouseOut events.
     */
    GameObject *lastMouseOverGO = nullptr;

public:
    SelectionFramebuffer(int width, int height);
    virtual ~SelectionFramebuffer();

    void RenderSelectionBuffer(const Scene *scene);
    void ProcessSelection();

    void OnChildAdded(GameObject *child) override;
};

#endif // SELECTIONFRAMEBUFFER_H
