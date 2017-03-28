#include "Bang/Object.h"

#include <typeinfo>

Object::Object()
{

}

Object::~Object()
{

}

void Object::CloneInto(ICloneable *clone) const
{
    Object *obj = Object::SCast<Object>(clone);
    obj->AddHideFlag( GetHideFlags() );
}

const HideFlags &Object::GetHideFlags() const
{
    return m_hideFlags;
}

bool Object::HasHideFlag(const HideFlags &hideFlag) const
{
    return (static_cast<int>(m_hideFlags) & static_cast<int>(hideFlag)) > 0;
}

void Object::AddHideFlag(const HideFlags &hideFlag)
{
    m_hideFlags = m_hideFlags | hideFlag;
}

void Object::RemoveHideFlag(const HideFlags &hideFlag)
{
    m_hideFlags = m_hideFlags ^ hideFlag;
}

String Object::GetInstanceId() const
{
    return String::ToString( static_cast<const void*>(this) );
}

