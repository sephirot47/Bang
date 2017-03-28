#include "Bang/Renderer.h"

#include <functional>

#include "Bang/AABox.h"
#include "Bang/Debug.h"
#include "Bang/Scene.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/SceneManager.h"
#include "Bang/AssetsManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/GraphicPipeline.h"

#ifdef BANG_EDITOR
#include "Bang/EditorScene.h"
#include "Bang/SelectionFramebuffer.h"
#endif

Renderer::Renderer()
{
    #ifdef BANG_EDITOR
    p_OnBindForSelectionFunc = [](){};
    #endif

    SetMaterial( AssetsManager::Load<Material>("Materials/G_Default.bmat", true));
}

Renderer::~Renderer()
{
    if (m_materialCopy)
    {
        delete m_materialCopy;
    }
}

void Renderer::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Renderer *r = Object::SCast<Renderer>(clone);
    r->SetMaterial(GetSharedMaterial());
    r->SetDrawWireframe(GetDrawWireframe());
    r->SetCullMode(GetCullMode());
    r->SetRenderMode(GetRenderMode());
    r->SetLineWidth(GetLineWidth());
    r->SetClosedInInspector(IsClosedInInspector());
    r->SetTransparent(IsTransparent());

}

void Renderer::SetMaterial(Material *m)
{
    m_material = m;
}

Material *Renderer::GetMaterial() const
{
    if (m_materialCopy) { return m_materialCopy; }
    return m_material ? m_material : Material::GetMissingMaterial();
}

Material *Renderer::GetSharedMaterial() const
{
    return m_material;
}

void Renderer::Bind() const
{
    GL::SetWireframe(m_drawWireframe);
    GL::SetCullMode(m_cullMode);
    glLineWidth(m_lineWidth);

    Matrix4 model; transform->GetLocalToWorldMatrix(&model);
    GL::SetModelMatrix(model);

    ShaderProgram *sp = GetMaterial()->GetShaderProgram();
    sp->Bind();
    GL::ApplyContextToShaderProgram(sp);

    GBuffer *gb = GraphicPipeline::GetActive()->GetGBuffer();
    gb->BindTextureBuffersTo(sp);
}

void Renderer::RenderForSelectionWithoutMaterial() const
{
    RenderWithoutMaterial();
}

void Renderer::Render() const
{
    RenderWithMaterial( GetMaterial() );
}

void Renderer::RenderWithMaterial(Material *_mat) const
{
    Bind();

    Material *mat = _mat ? _mat : Material::GetMissingMaterial();
    mat->Bind();

    #ifdef BANG_EDITOR
    GraphicPipeline *gp = GraphicPipeline::GetActive();
    SelectionFramebuffer *sfb = gp->GetSelectionFramebuffer();
    if (sfb->IsPassing())
    {
        p_OnBindForSelectionFunc();
        RenderForSelectionWithoutMaterial();
    }
    else
    #endif
    {
        RenderWithoutMaterial();
    }

    UnBind();
}

void Renderer::UseMaterialCopy()
{
    if (m_materialCopy) { delete m_materialCopy; }
    if (GetSharedMaterial())
    {
        m_materialCopy = new Material( *GetSharedMaterial() );
    }
}

void Renderer::UnBind() const
{
    ShaderProgram *sp = GetMaterial()->GetShaderProgram();
    sp->UnBind();
}

void Renderer::SetTransparent(bool transparent)
{
    m_isTransparent = transparent;
}
bool Renderer::IsTransparent() const
{
    return m_isTransparent;
}

void Renderer::SetDepthLayer(Renderer::DepthLayer dl)
{
    m_depthLayer = dl;
}

Renderer::DepthLayer Renderer::GetDepthLayer() const
{
    return m_depthLayer;
}

void Renderer::SetDrawWireframe(bool drawWireframe)
{
    m_drawWireframe = drawWireframe;
}
bool Renderer::GetDrawWireframe() const
{
    return m_drawWireframe;
}

Rect Renderer::GetBoundingRect(Camera *camera) const
{
    return camera->GetScreenBoundingRect(GetAABBox());
}


void Renderer::SetCullMode(GL::CullMode cullMode)
{
    m_cullMode = cullMode;
}
GL::CullMode Renderer::GetCullMode() const
{
    return m_cullMode;
}


void Renderer::SetRenderMode(GL::RenderMode renderMode)
{
    m_renderMode = renderMode;
}
GL::RenderMode Renderer::GetRenderMode() const
{
    return m_renderMode;
}


void Renderer::SetLineWidth(float w)
{
    m_lineWidth = w;
}
float Renderer::GetLineWidth() const
{
    return m_lineWidth;
}

#ifdef BANG_EDITOR
void Renderer::SetBindForSelectionFunction(
        const std::function<void()> &f)
{
    p_OnBindForSelectionFunc = f;
}
#endif

void Renderer::Read(const XMLNode &xmlInfo)
{
    Component::Read(xmlInfo);

    String materialFilepath = xmlInfo.GetFilepath("Material");
    if (!materialFilepath.Empty())
    {
        SetMaterial( AssetsManager::Load<Material>(materialFilepath) );
    }
    else
    {
        SetMaterial (nullptr);
    }

    SetTransparent(xmlInfo.GetBool("IsTransparent"));
    SetLineWidth(xmlInfo.GetFloat("LineWidth"));
    SetDrawWireframe(xmlInfo.GetBool("DrawWireframe"));
}

void Renderer::Write(XMLNode *xmlInfo) const
{
    Component::Write(xmlInfo);

    Material *sharedMat = GetSharedMaterial();
    if (sharedMat)
    {
        if (sharedMat->GetFilepath() != "")
        {
            xmlInfo->SetFilepath("Material", sharedMat->GetFilepath(), "bmat");
        }
        else //In case the asset is created in runtime, write its mem address
        {
            String memAddress = String::ToString((void*) sharedMat);
            xmlInfo->SetFilepath("Material", memAddress, "bmat");
        }
    }
    else
    {
        xmlInfo->SetFilepath("Material", "", "bmat");
    }

    xmlInfo->SetFloat("LineWidth", GetLineWidth());
    xmlInfo->SetBool("IsTransparent", IsTransparent(), {XMLProperty::Inline});
    xmlInfo->SetBool("DrawWireframe", GetDrawWireframe(), {XMLProperty::Inline});
}
