#ifndef ENTITY_H
#define ENTITY_H

#include <list>
#include <string>
#include <iostream>
#include <functional>

#include "IToString.h"
#include "StageEventListener.h"

class Part;
class Stage;
class Entity : public StageEventListener, public IToString
{
friend class Canvas;
friend class Stage;
private:
    std::string name;
    std::list<Part*> parts;
    std::list<Entity*> children;
    Entity* parent;

    virtual void _OnStart() override;
    virtual void _OnUpdate() override;
    virtual void _OnRender() override;
    virtual void _OnDestroy() override;

public:
    Entity();
    Entity(const std::string &name);

    virtual ~Entity();


    void AddChild(Entity *child);
    Entity* GetChild(const std::string &name) const;
    void RemoveChild(const std::string &name);

    void SetParent(Entity *parent);

    const std::string ToString() const;

    Stage* GetStage() const;
    Entity* GetParent() const;
    const std::string GetName() const { return name; }
    const std::list<Part*>* GetParts() const { return &parts; }
    const std::list<Entity*>* GetChildren() const { return &children; }


    template <class T>
    T* AddPart()
    {
        T *part = new T();
        parts.push_back(part);
        part->parent = this;
        return part;
    }

    template <class T>
    T* GetPart() const
    {
        for(auto part = parts.begin(); part != parts.end(); ++part)
        {
            T *tp = dynamic_cast<T*>(*part);
            if(tp != nullptr)
            {
                return tp;
            }
        }
        return nullptr;
    }

    template <class T>
    bool HasPart() const
    {
        return GetPart<T>() != nullptr;
    }

    template <class T>
    void RemovePart()
    {
        for(auto part = parts.begin(); part != parts.end(); ++part)
        {
            T *tp = dynamic_cast<T*>(*part);
            if(tp != nullptr)
            {
                parts.erase(part);
                delete tp;
                break;
            }
        }
    }
};

#endif // ENTITY_H
