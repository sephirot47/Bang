#include "Bang/Texture2D.h"

#include "Bang/XMLNode.h"

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

String Texture2D::GetFileExtensionStatic()
{
    return "btex2d";
}

String Texture2D::GetFileExtension() const
{
    return Texture2D::GetFileExtensionStatic();
}

void Texture2D::Read(const XMLNode &xmlInfo)
{
    Asset::Read(xmlInfo);

    Path imageFilepath = xmlInfo.GetFilepath("ImageFilepath");
    LoadFromImage(imageFilepath);

    String filterModeString = xmlInfo.GetEnumSelectedName("FilterMode");
    G_Texture::FilterMode filterMode = FilterMode::Nearest;
    if (filterModeString == "Nearest") { filterMode = FilterMode::Nearest; }
    else if (filterModeString == "Linear") { filterMode = FilterMode::Linear; }
    else if (filterModeString == "Trilinear")
    {
        filterMode = FilterMode::Trilinear;
    }
    SetFilterMode(filterMode);

    SetAlphaCutoff(xmlInfo.GetFloat("AlphaCutoff"));
}

void Texture2D::Write(XMLNode *xmlInfo) const
{
    Asset::Write(xmlInfo);

    xmlInfo->SetFilepath("ImageFilepath", m_imageFilepath,
                         "jpg png bmp tga tiff gif bmp");

    int selectedIndex = GetFilterMode();
    if (GetFilterMode() == FilterMode::Nearest)        { selectedIndex = 0; }
    else if (GetFilterMode() == FilterMode::Linear)    { selectedIndex = 1; }
    else if (GetFilterMode() == FilterMode::Trilinear) { selectedIndex = 2; }
    xmlInfo->SetEnum("FilterMode", {"Nearest", "Linear", "Trilinear"},
                     selectedIndex, {});

    xmlInfo->SetFloat("AlphaCutoff", GetAlphaCutoff());
}