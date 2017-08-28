#ifndef UIRENDERER_H
#define UIRENDERER_H

#include "Bang/Color.h"
#include "Bang/Renderer.h"
#include "Bang/Component.h"

class UIRenderer : public Renderer
{
    COMPONENT(UIRenderer)

protected:
    UIRenderer();
    virtual ~UIRenderer();
};

#endif // UIRENDERER_H
