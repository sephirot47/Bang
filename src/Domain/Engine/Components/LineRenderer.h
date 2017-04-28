#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/String.h"
#include "Bang/Vector3.h"
#include "Bang/MeshRenderer.h"

class Mesh;
class XMLNode;
class Material;
class ICloneable;
class LineRenderer : public Renderer
{
    OBJECT(LineRenderer)
    COMPONENT_ICON(LineRenderer, "Icons/eye.png")

public:
    virtual ~LineRenderer();

    virtual void CloneInto(ICloneable *clone) const override;

    const Array<Vector3>& GetPoints() const;
    virtual void SetPoint(int i, const Vector3& point);
    virtual void SetPoints(const Array<Vector3>& points);
    virtual AABox GetAABBox() const override;

    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;

protected:
    LineRenderer();

    virtual void RenderWithoutMaterial() const override;

private:
    MeshRenderer  *m_meshRenderer = nullptr;
    Array<Vector3> m_points;
};

#endif // LINERENDERER_H
