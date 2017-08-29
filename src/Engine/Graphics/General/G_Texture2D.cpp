#include "Bang/G_Texture2D.h"

#include "Bang/Resources.h"

G_Texture2D::G_Texture2D() : G_Texture(GL::TextureTarget::Texture2D)
{
    CreateEmpty(1,1);
}

G_Texture2D::~G_Texture2D()
{
}

void G_Texture2D::LoadFromImage(const G_Image &image)
{
    if (image.GetData())
    {
        m_width  = image.GetWidth();
        m_height = image.GetHeight();

        SetInternalFormat(GL::ColorInternalFormat::RGBA_UByte8);
        Fill(image.GetData(), m_width, m_height,
             GL::ColorInternalFormat::RGBA_UByte8);
    }
}

void G_Texture2D::CreateEmpty(int width, int height)
{
    int dataSize = width * height * GL::GetPixelBytesSize(m_internalFormat);
    Byte *data = new Byte[dataSize];
    memset(data, 0, dataSize);
    Fill(data, width, height, dataSize, true);
    delete[] data;
}

void G_Texture2D::Resize(int width, int height)
{
    CreateEmpty(width, height);
}

void G_Texture2D::Fill(const Byte *newData, int width, int height,
                       int sizeOfNewData, bool genMipMaps)
{
    GL::ClearError();
    if (m_data) { delete[] m_data; }

    uint dataSize = sizeOfNewData >= 0 ?
                    sizeOfNewData : (width * height * 16);
    m_data = new Byte[dataSize];
    memcpy(m_data, newData, dataSize); // Copy data
    m_width = width;
    m_height = height;

    Bind();
    GL::ClearError();
    glTexImage2D(GLCAST(m_target),
                 0,
                 GLCAST(GetInternalFormat()),
                 m_width,
                 m_height,
                 0,
                 GLCAST(GL::GetColorOrderFrom(GetInternalFormat())),
                 GLCAST(GetInternalDataType()),
                 m_data);
    GL::CheckError();
    if (genMipMaps && m_width > 0 && m_height > 0)
    {
        GenerateMipMaps();
    }
    UnBind();
}

void G_Texture2D::GenerateMipMaps() const
{
    Bind();
    glGenerateMipmap(GL_TEXTURE_2D);
    UnBind();
}

void G_Texture2D::Fill(const Byte *newData, int width, int height,
                     GL::ColorInternalFormat imageFormat,
                     bool genMipMaps)
{
    SetInternalFormat(imageFormat);

    int sizeOfNewData = width * height * GL::GetPixelBytesSize(imageFormat);
    Fill(newData, width, height, sizeOfNewData, genMipMaps);
}

G_Image G_Texture2D::ToImage(bool invertY)
{
    const int width  = GetWidth();
    const int height = GetHeight();
    const uint bytesSize = GetBytesSize();
    Byte *pixels = new Byte[bytesSize];

    glBindTexture(GL_TEXTURE_2D, GetGLId());
    glGetTexImage(GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  pixels);

    G_Image img(width, height);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            const int coords = (i * width + j) * 4;
            Color pixelColor = Color(pixels[coords + 0] / 255.0f,
                                     pixels[coords + 1] / 255.0f,
                                     pixels[coords + 2] / 255.0f,
                                     pixels[coords + 3] / 255.0f);
            img.SetPixel(j, i, pixelColor);
        }
    }
    if (invertY) { img.InvertVertically(); }

    delete[] pixels;

    return img;
}

void G_Texture2D::SetAlphaCutoff(float alphaCutoff)
{
    m_alphaCutoff = alphaCutoff;
}

float G_Texture2D::GetAlphaCutoff() const
{
    return m_alphaCutoff;
}
