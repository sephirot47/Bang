#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class Component;

class ComponentFactory
{
public:
    static Component* CreateComponent(const String &componentClassName);
    static bool ExistsComponentClass(const String &componentClassName);

    template<class T>
    static T* CreateComponent();

    ComponentFactory() = delete;
};

template<class T>
T* ComponentFactory::CreateComponent()
{
    return new T();
}

NAMESPACE_BANG_END

#endif // COMPONENTFACTORY_H
