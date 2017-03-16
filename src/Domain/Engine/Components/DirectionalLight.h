#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Math.h"
#include "Light.h"
#include "String.h"

class XMLNode;
class ICloneable;
class DirectionalLight : public Light
{
    OBJECT(DirectionalLight)
    ICLONEABLE(DirectionalLight)

public:
    virtual void CloneInto(ICloneable *clone) const override;

    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;

    virtual void OnDrawGizmos(bool depthed, bool overlay) override;

protected:
    DirectionalLight();
    virtual ~DirectionalLight();
};

#endif // DIRECTIONALLIGHT_H
