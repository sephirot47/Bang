﻿#include "Bang/GEngine.h"

#include "Bang/GL.h"
#include "Bang/VAO.h"
#include "Bang/VBO.h"
#include "Bang/Mesh.h"
#include "Bang/Scene.h"
#include "Bang/Light.h"
#include "Bang/Input.h"
#include "Bang/Window.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/ChronoGL.h"
#include "Bang/Material.h"
#include "Bang/Renderer.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/MeshFactory.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/RectTransform.h"
#include "Bang/UILayoutManager.h"
#include "Bang/MaterialFactory.h"
#include "Bang/TextureUnitManager.h"
#include "Bang/SelectionFramebuffer.h"

USING_NAMESPACE_BANG

GEngine* GEngine::s_gEngine = nullptr;

GEngine::GEngine()
{
}

GEngine::~GEngine()
{
    delete m_texUnitManager;
    delete m_gl;
}

void GEngine::Init()
{
    m_gl = new GL();
    GL::SetActive( GetGL() );
    m_texUnitManager = new TextureUnitManager();

    p_windowPlaneMesh = Resources::Clone<Mesh>(MeshFactory::GetUIPlane());
    p_renderTextureToViewportMaterial = MaterialFactory::GetRenderTextureToViewport();
    GL::SetActive( nullptr );
}

void GEngine::Render(GameObject *go, Camera *camera)
{
    if (!go) { return; }

    go->BeforeRender();

    SetActiveRenderingCamera(camera);
    RenderShadowMaps(go);
    SetActiveRenderingCamera(nullptr);

    SetActiveRenderingCamera(camera);
    RenderToGBuffer(go, camera);
    RenderToSelectionFramebuffer(go, camera);
    SetActiveRenderingCamera(nullptr);
}

void GEngine::ApplyStenciledDeferredLightsToGBuffer(GameObject *lightsContainer,
                                                    Camera *camera,
                                                    const AARect &maskRectNDC)
{
    Byte prevStencilValue              = GL::GetStencilValue();
    GL::Function prevStencilFunc       = GL::GetStencilFunc();
    GL::StencilOperation prevStencilOp = GL::GetStencilOp();

    // We have marked from before the zone where we want to apply the effect
    GL::SetStencilOp(GL::StencilOperation::Keep);
    GL::SetStencilFunc(GL::Function::Equal);
    GL::SetStencilValue(1);

    List<Light*> lights = lightsContainer->GetComponentsInChildren<Light>();
    for (Light *light : lights)
    {
        if (!light || !light->IsEnabled(true)) { continue; }
        light->ApplyLight(camera, maskRectNDC);
    }

    GL::SetStencilValue(prevStencilValue);
    GL::SetStencilFunc(prevStencilFunc);
    GL::SetStencilOp(prevStencilOp);
}

Camera *GEngine::GetActiveRenderingCamera()
{
    GEngine *ge = GEngine::GetActive();
    return ge ? ge->p_currentRenderingCamera : nullptr;
}

GBuffer *GEngine::GetActiveGBuffer()
{
    Camera *cam = GEngine::GetActiveRenderingCamera();
    return cam ? cam->GetGBuffer() : nullptr;
}

SelectionFramebuffer *GEngine::GetActiveSelectionFramebuffer()
{
    Camera *cam = GEngine::GetActiveRenderingCamera();
    return cam ? cam->GetSelectionFramebuffer() : nullptr;
}

void GEngine::RenderToGBuffer(GameObject *go, Camera *camera)
{
    camera->BindGBuffer();

    // GBuffer Scene rendering
    camera->GetGBuffer()->SetAllDrawBuffers();
    GL::SetDepthMask(true);
    GL::SetDepthFunc(GL::Function::LEqual);
    RenderWithPassAndMarkStencilForLights(go, RenderPass::Scene);
    ApplyStenciledDeferredLightsToGBuffer(go, camera);
    camera->GetGBuffer()->SetColorDrawBuffer();
    RenderWithPass(go, RenderPass::ScenePostProcess);

    // GBuffer Canvas rendering
    camera->GetGBuffer()->SetAllDrawBuffers();
    GL::Enablei(GL::Test::Blend, 0);
    GL::BlendFunc(GL::BlendFactor::SrcAlpha, GL::BlendFactor::OneMinusSrcAlpha);
    GL::ClearDepthBuffer();
    GL::SetDepthMask(true);
    GL::SetDepthFunc(GL::Function::LEqual);
    RenderWithPass(go, RenderPass::Canvas);
    camera->GetGBuffer()->SetColorDrawBuffer();
    RenderWithPass(go, RenderPass::CanvasPostProcess);

    // GBuffer Overlay rendering
    camera->GetGBuffer()->SetAllDrawBuffers();
    GL::ClearStencilBuffer();
    GL::ClearDepthBuffer();
    GL::SetDepthMask(true);
    GL::SetDepthFunc(GL::Function::LEqual);
    RenderWithPassAndMarkStencilForLights(go, RenderPass::Overlay);
    ApplyStenciledDeferredLightsToGBuffer(go, camera);
    RenderWithPass(go, RenderPass::OverlayPostProcess);
    GL::Disablei(GL::Test::Blend, 0);
}

void GEngine::RenderToSelectionFramebuffer(GameObject *go, Camera *camera)
{
    if (camera->GetRenderSelectionBuffer())
    {
        camera->BindSelectionFramebuffer();

        // Selection rendering
        camera->GetSelectionFramebuffer()->PrepareNewFrameForRender(go);
        go->Render(RenderPass::Scene);
        GL::ClearStencilBuffer();
        GL::ClearDepthBuffer();
        GL::SetDepthFunc(GL::Function::LEqual);
        RenderWithPass(go, RenderPass::Canvas);

        GL::ClearDepthBuffer();
        RenderWithPass(go, RenderPass::Overlay);
    }
}

void GEngine::RenderWithPass(GameObject *go, RenderPass renderPass)
{
    Camera *cam = GetActiveRenderingCamera();
    if (cam && cam->MustRenderPass(renderPass))
    {
        RenderWithPassRaw(go, renderPass);
    }
}

void GEngine::RenderWithPassRaw(GameObject *go, RenderPass renderPass)
{
    go->Render(renderPass, true);
}

void GEngine::RenderWithPassAndMarkStencilForLights(GameObject *go,
                                                    RenderPass renderPass)
{
    Byte prevStencilValue                     = GL::GetStencilValue();
    GL::StencilOperation prevStencilOperation = GL::GetStencilOp();

    GL::SetStencilValue(1);
    GL::SetStencilOp(GL::StencilOperation::Replace);

    // Render pass
    RenderWithPass(go, renderPass);

    // Restore
    GL::SetStencilOp(prevStencilOperation);
    GL::SetStencilValue(prevStencilValue);
}

void GEngine::SetActive(GEngine *gEngine)
{
    GEngine::s_gEngine = gEngine;
    GL::SetActive( gEngine ? gEngine->GetGL() : nullptr );
}


void GEngine::RenderViewportRect(ShaderProgram *sp, const AARect &destRectMask)
{
    // Save state
    GLId prevBoundShaderProgram = GL::GetBoundId(GL::BindTarget::ShaderProgram);

    // Set state
    sp->Bind();
    sp->Set("B_UvOffset",         Vector2::Zero, false);
    sp->Set("B_UvMultiply",       Vector2::One, false);
    sp->Set("B_destRectMinCoord", destRectMask.GetMin(), false);
    sp->Set("B_destRectMaxCoord", destRectMask.GetMax(), false);
    RenderViewportPlane();

    // Restore state
    GL::Bind(GL::BindTarget::ShaderProgram, prevBoundShaderProgram);
}

void GEngine::RenderTexture(Texture2D *texture)
{
    // if (!cam) { return; }
    p_renderTextureToViewportMaterial.Get()->Bind();

    ShaderProgram *sp = p_renderTextureToViewportMaterial.Get()->GetShaderProgram();
    sp->Set(GBuffer::GetColorsTexName(), texture, false);

    GEngine::RenderViewportRect(sp, AARect::NDCRect);

    p_renderTextureToViewportMaterial.Get()->UnBind();
}

void GEngine::RenderViewportPlane()
{
    // Save state
    bool prevWireframe         = GL::IsWireframe();
    bool prevDepthMask         = GL::GetDepthMask();
    GL::Function prevDepthFunc = GL::GetDepthFunc();

    // Set state
    GL::SetWireframe(false);
    GL::SetDepthFunc(GL::Function::Always);
    GL::SetDepthMask(false);

    GL::Render(p_windowPlaneMesh.Get()->GetVAO(), GL::Primitive::Triangles,
               p_windowPlaneMesh.Get()->GetVertexCount());

    // Restore state
    GL::SetWireframe(prevWireframe);
    GL::SetDepthMask(prevDepthMask);
    GL::SetDepthFunc(prevDepthFunc);
}

GEngine* GEngine::GetActive()
{
    Window *win = Window::GetActive();
    return win ? win->GetGEngine() : nullptr;
}

void GEngine::RenderShadowMaps(GameObject *go)
{
    if (!go->IsActive()) { return; }

    List<Light*> lights = go->GetComponentsInChildren<Light>(true);
    for (Light *light : lights)
    {
        if (light->IsActive()) { light->RenderShadowMaps(); }
    }
}

void GEngine::SetActiveRenderingCamera(Camera *camera)
{
    if (p_currentRenderingCamera) { p_currentRenderingCamera->UnBind(); }

    p_currentRenderingCamera = camera;
    if (p_currentRenderingCamera) { p_currentRenderingCamera->Bind(); }
}

void GEngine::Render(Renderer *rend)
{
    Camera *activeCamera = GetActiveRenderingCamera();
    if (activeCamera)
    {
        if (GL::IsBound(activeCamera->GetSelectionFramebuffer()))
        {
            activeCamera->GetSelectionFramebuffer()->RenderForSelectionBuffer(rend);
        }
        else
        {
            ASSERT( GL::IsBound(activeCamera->GetGBuffer()) ||
                    GL::GetBoundId(GL::BindTarget::DrawFramebuffer) > 0 );
            GEngine::RenderRaw(rend);
        }
    }
    else
    {
        GEngine::RenderRaw(rend);
    }
}

void GEngine::RenderRaw(Renderer *rend)
{
    rend->Bind();
    rend->OnRender();
    rend->UnBind();
}

GL *GEngine::GetGL() const { return m_gl; }
TextureUnitManager *GEngine::GetTextureUnitManager() const
{
    return m_texUnitManager;
}
