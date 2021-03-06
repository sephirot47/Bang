#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Bang/Asset.h"
#include "Bang/Image.h"
#include "Bang/Texture.h"

NAMESPACE_BANG_BEGIN

class Texture2D : public Texture,
                  public Asset
{
    ASSET(Texture2D)

public:
    Texture2D(const Texture2D &tex) = delete;

    void CreateEmpty(int width, int height) override;
    void Resize(int width, int height) override;
    void Fill(const Color &fillColor,
              int width, int height);
    void Fill(const Byte *newData,
              int width, int height,
              GL::ColorComp inputDataColorComp,
              GL::DataType inputDataType);
    void GenerateMipMaps() const;

    template<class T = Byte>
    Image<T> ToImage(bool invertY = false) const;

    void SetAlphaCutoff(float alphaCutoff);
    float GetAlphaCutoff() const;

    void Import(const Image<Byte> &image);

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

    // Resource
    virtual void Import(const Path &imageFilepath) override;

protected:
    float m_alphaCutoff = 0.1f;

    Texture2D();
    virtual ~Texture2D();

private:
    static Color GetColorFromArray(const float *pixels, int i);
    static Color GetColorFromArray(const Byte *pixels, int i);

    void GetTexImageInto(float *pixels) const;
    void GetTexImageInto(Byte *pixels) const;
    int GetNumComponents() const;
};

template<class T>
Image<T> Texture2D::ToImage(bool invertY) const
{
    const int width  = GetWidth();
    const int height = GetHeight();
    const int numComps = GL::GetNumComponents( GL::ColorComp::RGBA );
    T *pixels = new T[width * height * numComps];

    GetTexImageInto(pixels);

    Image<T> img(width, height);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const int coords = (y * width + x) * numComps;
            Color pixelColor = GetColorFromArray(pixels, coords);
            img.SetPixel(x, y, pixelColor);
        }
    }
    if (invertY) { img.InvertVertically(); }

    delete[] pixels;

    return img;
}

NAMESPACE_BANG_END

#endif // TEXTURE2D_H
