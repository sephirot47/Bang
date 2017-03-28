#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/GLObject.h"
#include "Bang/NamedEnum.h"
#include "Bang/IToString.h"

class Shader : public GLObject,
               public IToString
{
public:
    enum class Type
    {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER
    };

private:
    String m_sourceCode = "";
    String m_filepath = "";
    Type m_type;

public:
    Shader(Shader::Type t);
    Shader(Shader::Type t, const String &shaderPath);

    bool LoadFromFile(const String &filepath);

    GL::BindTarget GetGLBindTarget() const override;

    const String& GetSourceCode() const;
    const String& GetFilepath() const;
    Type GetType() const;
    String ToString() const;
};

#endif // SHADER_H
