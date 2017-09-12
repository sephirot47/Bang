#include "Bang/Renderer.h"

#include <functional>

#include "Bang/Rect.h"
#include "Bang/AABox.h"
#include "Bang/Camera.h"
#include "Bang/Transform.h"
#include "Bang/GBuffer.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/MaterialFactory.h"
#include "Bang/GraphicPipeline.h"

USING_NAMESPACE_BANG

Renderer::Renderer()
{
    SetMaterial( MaterialFactory::GetDefault() );
}

Renderer::~Renderer()
{
    if (m_materialCopy) { delete m_materialCopy; }
}

void Renderer::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);
    if (IsVisible() && renderPass == GetRenderPass())
    {
        GraphicPipeline::GetActive()->Render(this);
    }
}
void Renderer::OnRender() { }

void Renderer::Bind() const
{
    GL::SetViewProjMode(m_viewProjMode);
    GL::SetWireframe(m_drawWireframe);
    GL::SetCullFace(m_cullMode);
    GL::LineWidth(m_lineWidth);

    Matrix4 model;
    if (gameObject && gameObject->transform)
    {
        gameObject->transform->GetLocalToWorldMatrix(&model);
    }
    GL::SetModelMatrix(model);

    Material *mat = GetMaterial();
    mat->Bind();
}

void Renderer::UnBind() const
{
    GetMaterial()->UnBind();
}

void Renderer::UseMaterialCopy()
{
    if (m_materialCopy)
    {
        delete m_materialCopy;
        m_materialCopy = nullptr;
    }

    if (GetSharedMaterial())
    {
        m_materialCopy = new Material();
        GetSharedMaterial()->CloneInto(m_materialCopy);
    }
}

void Renderer::SetVisible(bool visible)
{
    m_visible = visible;
}


void Renderer::SetMaterial(Material *m)
{
    if (m_material != m)
    {
        m_material = m;
        if (m_materialCopy)
        {
            delete m_materialCopy;
            m_materialCopy = nullptr;
        }
    }
}

void Renderer::SetRenderPass(RenderPass rp) { m_renderPass = rp; }

bool Renderer::IsVisible() const { return m_visible; }

bool Renderer::NeedsReadingColorBuffer() const
{
    Material *mat = GetMaterial();
    return mat ? (mat->GetDiffuseColor().a < 1) : false;
}
RenderPass Renderer::GetRenderPass() const { return m_renderPass; }
bool Renderer::IsRenderWireframe() const { return m_drawWireframe; }
AABox Renderer::GetAABBox() const { return AABox(); }
void Renderer::SetCullMode(GL::Face cullMode) { m_cullMode = cullMode; }
GL::Face Renderer::GetCullMode() const { return m_cullMode; }
void Renderer::SetLineWidth(float w) { m_lineWidth = w; }
void Renderer::SetRenderWireframe(bool drawWireframe)
{
    m_drawWireframe = drawWireframe;
}
void Renderer::SetViewProjMode(GL::ViewProjMode viewProjMode)
{
    m_viewProjMode = viewProjMode;
}
void Renderer::SetRenderPrimitive(GL::Primitives renderMode)
{
    m_renderMode = renderMode;
}

GL::ViewProjMode Renderer::GetViewProjMode() const { return m_viewProjMode; }
GL::Primitives Renderer::GetRenderPrimitive() const { return m_renderMode; }
float Renderer::GetLineWidth() const { return m_lineWidth; }
Material *Renderer::GetSharedMaterial() const { return m_material; }
Material *Renderer::GetMaterial() const
{
    if (m_materialCopy) { return m_materialCopy; }
    return m_material ? m_material : MaterialFactory::GetMissing();
}
Rect Renderer::GetBoundingRect(Camera *camera) const
{
    return Rect::ScreenRect;
    return camera ? camera->GetScreenBoundingRect(GetAABBox()) :
                    Rect::Zero;
}

void Renderer::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Renderer *r = SCAST<Renderer*>(clone);
    r->SetMaterial(GetSharedMaterial());
    r->SetRenderWireframe(IsRenderWireframe());
    r->SetCullMode(GetCullMode());
    r->SetRenderPrimitive(GetRenderPrimitive());
    r->SetLineWidth(GetLineWidth());
}

void Renderer::ImportXML(const XMLNode &xml)
{
    Component::ImportXML(xml);

    if (xml.Contains("Visible"))
    { SetVisible( xml.Get<bool>("Visible") ); }

    if (xml.Contains("Material"))
    { SetMaterial( Resources::Load<Material>( xml.Get<GUID>("Material") ) ); }

    if (xml.Contains("RenderPass"))
    { SetRenderPass( xml.Get<RenderPass>("RenderPass") ); }

    if (xml.Contains("LineWidth"))
    { SetLineWidth(xml.Get<float>("LineWidth")); }

    if (xml.Contains("RenderWireframe"))
    { SetRenderWireframe(xml.Get<bool>("RenderWireframe")); }
}

void Renderer::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    Material *sharedMat = GetSharedMaterial();
    xmlInfo->Set("Visible", IsVisible());
    xmlInfo->Set("Material", sharedMat ? sharedMat->GetGUID() : GUID::Empty());
    xmlInfo->Set("RenderPass", GetRenderPass());
    xmlInfo->Set("LineWidth", GetLineWidth());
    xmlInfo->Set("RenderWireframe", IsRenderWireframe());
}
