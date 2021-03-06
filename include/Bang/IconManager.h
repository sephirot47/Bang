#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture2D;

class IconManager
{
public:
    static RH<Texture2D> GetRightArrowIcon();
    static RH<Texture2D> GetDownArrowIcon();
    static RH<Texture2D> GetCheckIcon();
    static RH<Texture2D> GetAudioIcon();
    static RH<Texture2D> GetSunIcon();
    static RH<Texture2D> GetLightBulbIcon();

    static RH<Texture2D> GetIconTexture(const String &filename, const Path &dir);

private:
    IconManager() = default;
    virtual ~IconManager() = default;

    static RH<Texture2D> GetIconTexture(const String &filename);
};

NAMESPACE_BANG_END

#endif // ICONMANAGER_H

