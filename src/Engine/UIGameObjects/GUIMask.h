#ifndef UIMASK_H
#define UIMASK_H

#include "Bang/GL.h"
#include "Bang/UIGameObject.h"

class GUIMask : public UIGameObject
{
public:
    GUIMask();
    virtual ~GUIMask();

    virtual void Render(RenderPass renderPass) override;
    virtual void OnRender(RenderPass renderPass) override;

    void SetDrawMask(bool drawMask);
    bool GetDrawMask() const;

private:
    bool m_drawMask = false;

    GLenum m_stencilOpBefore;
    bool m_maskRBefore = true, m_maskGBefore = true,
         m_maskBBefore = true, m_maskABefore = true;
    bool m_stencilTestBefore = false;
    bool m_stencilWriteBefore = false;

    void BeforeThisRender();
    void BeforeChildrenRender();
    void AfterChildrenRender();
};

#endif // UIMASK_H
