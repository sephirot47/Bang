#ifndef TEXTURERENDER_H
#define TEXTURERENDER_H

#include "Texture.h"

class TextureRender : public Texture
{

public:
    TextureRender();
    virtual ~TextureRender();

    void CreateEmpty(int width, int height) override;
    void Resize(int width, int height) override;
    void Fill(unsigned char *newData, int width, int height, int numComponents);
};

#endif // TEXTURERENDER_H
