#include "Prefab.h"

#include "XMLParser.h"
#include "GameObject.h"
#include "UIGameObject.h"

Prefab::Prefab()
{

}

Prefab::Prefab(const Prefab &p)
{
    m_gameObjectXMLInfoContent = p.m_gameObjectXMLInfoContent;
}

Prefab::Prefab(GameObject *go)
{
    if (go)
    {
        m_gameObjectXMLInfoContent = go->GetXMLInfoString();
    }
}

Prefab::Prefab(const String &gameObjectXMLInfoContent)
{
    m_gameObjectXMLInfoContent = gameObjectXMLInfoContent;
}

String Prefab::GetFileExtensionStatic()
{
    return "bprefab";
}

String Prefab::GetFileExtension()
{
    return Prefab::GetFileExtensionStatic();
}

GameObject *Prefab::Instantiate() const
{
    GameObject *go = InstantiateWithoutStarting();
    if (go)
    {
        go->_OnStart();
    }
    return go;
}

GameObject *Prefab::InstantiateWithoutStarting() const
{
    if (m_gameObjectXMLInfoContent != "")
    {
        XMLNode *xmlInfo = XMLParser::FromString(m_gameObjectXMLInfoContent);
        GameObject *go = xmlInfo->GetTagName() == "UIGameObject" ?
                            new UIGameObject() : new GameObject();
        go->ReadXMLInfo(xmlInfo);
        return go;
    }
    return nullptr;
}

void Prefab::ReadXMLInfo(const XMLNode *xmlInfo)
{
    Asset::ReadXMLInfo(xmlInfo);
    m_gameObjectXMLInfoContent = xmlInfo->ToString();
}

void Prefab::FillXMLInfo(XMLNode *xmlInfo) const
{
    Asset::FillXMLInfo(xmlInfo);

    XMLNode *goInfo = XMLParser::FromString(m_gameObjectXMLInfoContent);
    if (goInfo)
    {
        goInfo->CloneInto(xmlInfo);
        xmlInfo->SetTagName("Prefab");
        delete goInfo;
    }
}
