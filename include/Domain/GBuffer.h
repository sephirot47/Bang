#ifndef GBUFFER_H
#define GBUFFER_H

#include "Material.h"
#include "MeshScreenPlane.h"
#include "Framebuffer.h"

class GBuffer : public Framebuffer
{
public:
    enum Attachment
    {
        Position = 0,
        Normal,
        Uv,
        Diffuse,
        Depth
    };

private:

    Material *renderToScreenMaterial;
    MeshScreenPlane *renderToScreenPlaneMesh;

public:
    GBuffer(int width, int height);
    virtual ~GBuffer();

    void RenderToScreen() const;
};

#endif // GBUFFER_H
