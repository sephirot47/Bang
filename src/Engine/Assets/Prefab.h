#ifndef BANGFAB_H
#define BANGFAB_H

#include "Bang/Asset.h"
#include "Bang/String.h"

//Represents an GameObject that can be saved as an asset, and
//can be instantiated.
//In fact its asset file will be of the form
//<GameObject>
//  ...
//</GameObject>

class Prefab : public Asset
{
	SOBJECT(Prefab)

public:
    Prefab();
    Prefab(const Prefab &p);
    Prefab(GameObject *go);
    Prefab(const String &gameObjectXMLInfoContent);
    virtual ~Prefab();

    GameObject* Instantiate() const;
    GameObject* InstantiateWithoutStarting() const;

    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;

private:
    String m_gameObjectXMLInfoContent = "";
};

#endif // BANGFAB_H
