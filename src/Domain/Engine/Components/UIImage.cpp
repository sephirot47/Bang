﻿#include "UIImage.h"

#include "Rect.h"
#include "GBuffer.h"
#include "Material.h"
#include "Texture2D.h"
#include "GameObject.h"
#include "MeshFactory.h"
#include "ShaderProgram.h"
#include "AssetsManager.h"
#include "GraphicPipeline.h"

UIImage::UIImage()
{
    m_material = AssetsManager::Load<Material>("Materials/UI/G_UIImage.bmat",
                                               true);
    m_materialSP = AssetsManager::Load<Material>("Materials/UI/SP_UIImage.bmat",
                                                 true);
}

UIImage::~UIImage()
{
}

String UIImage::GetName() const
{
    return "UIImage";
}

void UIImage::CloneInto(ICloneable *clone) const
{
    UIRenderer::CloneInto(clone);
    UIImage *img = Object::SCast<UIImage>(clone);
    img->SetTexture( GetTexture() );
}

ICloneable *UIImage::Clone() const
{
    UIImage *img = new UIImage();
    CloneInto(img);
    return img;
}

void UIImage::RenderCustomSP() const
{
    /*
    UIRenderer::RenderCustomSP();

    ASSERT(m_material); ASSERT(m_materialSP);

    ShaderProgram *sp = m_materialSP->GetShaderProgram();
    sp->SetUniformColor("B_material_diffuse_color", m_tint);
    sp->SetUniformFloat("B_hasTexture",  m_imageTexture ? 1 : 0);
    sp->SetUniformTexture("B_texture_0", m_imageTexture);

    AABox screenBox = gameObject->GetAABBox();
    Rect renderRect(screenBox.GetMin().xy(), screenBox.GetMax().xy());
    GBuffer *gb = GraphicPipeline::GetActive()->GetGBuffer();
    gb->RenderPassWithMaterial(m_materialSP, renderRect);
    */
}

void UIImage::OnJustBeforeRendering(Material *mat) const
{
    UIRenderer::OnJustBeforeRendering(mat);
    if (mat == m_material || mat == m_materialSP)
    {
        m_material->SetTexture(m_imageTexture);
        m_materialSP->SetTexture(m_imageTexture);
    }
}

void UIImage::SetTexture(Texture2D *imageTexture)
{
    m_imageTexture = imageTexture;
}

Texture2D *UIImage::GetTexture() const
{
    return m_imageTexture;
}

void UIImage::ReadXMLInfo(const XMLNode *xmlInfo)
{
    UIRenderer::ReadXMLInfo(xmlInfo);

    String texFilepath = xmlInfo->GetFilepath("Image");
    m_imageTexture = AssetsManager::Load<Texture2D>(texFilepath);
}

void UIImage::FillXMLInfo(XMLNode *xmlInfo) const
{
    UIRenderer::FillXMLInfo(xmlInfo);
    xmlInfo->SetTagName(GetName());

    String texFilepath = m_imageTexture ? m_imageTexture->GetFilepath() : "";
    xmlInfo->SetFilepath("Image", texFilepath,
                         Texture2D::GetFileExtensionStatic(), {});

    xmlInfo->GetAttribute("Mesh")->SetProperty({XMLProperty::Hidden});
    xmlInfo->GetAttribute("Material")->SetProperty({XMLProperty::Hidden});
    xmlInfo->GetAttribute("LineWidth")->SetProperty({XMLProperty::Hidden});
    xmlInfo->GetAttribute("IsTransparent")->SetProperty({XMLProperty::Hidden});
    xmlInfo->GetAttribute("DrawWireframe")->SetProperty({XMLProperty::Hidden});
}

