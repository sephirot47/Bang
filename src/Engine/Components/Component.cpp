#include "Bang/Component.h"

#include "Bang/String.h"
#include "Bang/XMLNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"

Component::Component()
{
}

Component::~Component()
{
}

void Component::SetGameObject(GameObject *gameObject)
{
    m_gameObject = gameObject;
    m_gameObjectTransform = m_gameObject->transform;
}

GameObject *Component::GetGameObject() const { return m_gameObject; }

void Component::CloneInto(ICloneable *clone) const
{
    SerializableObject::CloneInto(clone);
    Component *c = SCAST<Component*>(clone);
    c->SetEnabled( IsEnabled() );
}

String Component::ToString() const
{
    std::ostringstream msg;
    msg << "";
    return GetClassName() + "(" + String::ToString((void*)this) + ")";
}

String Component::GetInstanceId() const
{
    String instanceId = "Component";
    if (gameObject)
    {
        instanceId.Prepend(gameObject->GetInstanceId() + "_");
        Component *ncThis = const_cast<Component*>(this);
        int indInGameObject = gameObject->GetComponents().IndexOf(ncThis);
        instanceId.Append( String::ToString(indInGameObject) );
    }
    return instanceId;
}

void Component::Read(const XMLNode &xmlInfo)
{
    SerializableObject::Read(xmlInfo);
    if (xmlInfo.Contains("Enabled"))
    { SetEnabled(xmlInfo.Get<bool>("Enabled", true)); }
}

void Component::Write(XMLNode *xmlInfo) const
{
    SerializableObject::Write(xmlInfo);

    xmlInfo->SetTagName( GetClassName() );
    xmlInfo->Set("Enabled", IsEnabled());
}
