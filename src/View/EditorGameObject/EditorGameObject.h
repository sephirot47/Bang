#ifndef EDITORGAMEOBJECT_H
#define EDITORGAMEOBJECT_H

#include "GameObject.h"

class EditorGameObject : public GameObject
{
public:
    EditorGameObject();
    EditorGameObject(const std::string &name);

    virtual bool IsEditorGameObject() const override;
};

#endif // EDITORGAMEOBJECT_H
