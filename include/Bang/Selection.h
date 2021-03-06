#ifndef SELECTION_H
#define SELECTION_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;
FORWARD class Camera;
FORWARD class SelectionFramebuffer;

class Selection
{
public:
    static GameObject *GetOveredGameObject();
    static GameObject *GetOveredGameObject(Scene *scene);
    static GameObject *GetOveredGameObject(Camera *camera);
    Selection() = delete;
};

NAMESPACE_BANG_END

#endif // SELECTION_H
