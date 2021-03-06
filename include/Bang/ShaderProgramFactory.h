#ifndef SHADERPROGRAMFACTORY_H
#define SHADERPROGRAMFACTORY_H

#include "Bang/ShaderProgram.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

class ShaderProgramFactory
{
public:
    static Path GetDefaultVertexShaderPath();
    static Path GetDefaultFragmentShaderPath();
    static Path GetPostProcessVertexShaderPath();

    static ShaderProgram* GetDefault();
    static ShaderProgram* GetDefaultPostProcess();
    static ShaderProgram* Get(const Path &vShaderPath, const Path &fShaderPath);

private:
    Map< std::pair<Path, Path>, RH<ShaderProgram> > m_cache;

    ShaderProgramFactory() = default;

    static ShaderProgramFactory* GetActive();

    friend class Resources;
};

NAMESPACE_BANG_END

#endif // SHADERPROGRAMFACTORY_H

