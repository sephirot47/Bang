#include "Bang/Texture2D.h"

#include "Bang/XMLNode.h"
#include "Bang/Resources.h"

Texture2D::Texture2D() : G_Texture2D()
{
}

Texture2D::Texture2D(const Path &imageFilepath) : G_Texture2D(imageFilepath)
{
}

Texture2D::Texture2D(const G_Texture2D *gTexture) : G_Texture2D(*gTexture)
{
}

Texture2D::~Texture2D()
{
}

void Texture2D::Read(const XMLNode &xmlInfo)
{
    Asset::Read(xmlInfo);

    G_Image *img = Resources::Load<G_Image>( xmlInfo.Get<GUID>("Image") );
    if (img) { LoadFromImage(*img); }

    SetFilterMode( xmlInfo.Get<FilterMode>("FilterMode") );
    SetAlphaCutoff( xmlInfo.Get<float>("AlphaCutoff") );
}

void Texture2D::Write(XMLNode *xmlInfo) const
{
    Asset::Write(xmlInfo);

    xmlInfo->Set("Image", p_image ? p_image->GetGUID() : GUID::Empty());

    xmlInfo->Set("FilterMode", GetFilterMode());
    xmlInfo->Set("AlphaCutoff", GetAlphaCutoff());
}
