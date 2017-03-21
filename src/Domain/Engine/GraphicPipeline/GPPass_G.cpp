#include "GPPass_G.h"

#include "Debug.h"
#include "Scene.h"
#include "GBuffer.h"
#include "Material.h"
#include "Renderer.h"
#include "GraphicPipeline.h"

GPPass_G::GPPass_G(GraphicPipeline *graphicPipeline,
                   bool receiveLighting, bool transparentPass,
                   const List<GPPass*> &subPasses)
    : GPPass(graphicPipeline, subPasses)
{
    m_receiveLighting = receiveLighting;
    m_transparentPass = transparentPass;
}

void GPPass_G::InPass(const List<Renderer*> &renderers,
                      const List<GameObject*> &sceneChildren)
{
    GPPass::InPass(renderers, sceneChildren);

    p_gbuffer->SetAllDrawBuffers();
    //if (m_transparentPass) { GL::SetWriteDepth(false); }

    p_gbuffer->SetStencilTest(false);
    p_gbuffer->SetStencilWrite(true);
    for (Renderer *rend : renderers)
    {
        if (!CanRender(rend)) { continue; }
        if (rend->IsTransparent())
        {
            p_gbuffer->PrepareColorReadBuffer(
                        rend->GetMaterial()->GetShaderProgram());
            p_gbuffer->SetAllDrawBuffers();
        }
        rend->Render();
    }
    //if (m_transparentPass) { GL::SetWriteDepth(true); }
}

bool GPPass_G::CanRender(const Renderer *renderer) const
{
    Material *rendMaterial = renderer->GetMaterial();
    bool receivesLighting = rendMaterial && rendMaterial->ReceivesLighting();
    return GPPass::CanRender(renderer) &&
           (m_transparentPass == renderer->IsTransparent()) &&
           (m_receiveLighting == receivesLighting);
}