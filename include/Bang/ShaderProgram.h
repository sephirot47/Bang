#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <unordered_map>

#include "Bang/Map.h"
#include "Bang/GLObject.h"
#include "Bang/Resources.h"
#include "Bang/IDestroyListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class Shader;
FORWARD class Texture2D;

class ShaderProgram : public GLObject,
                      public IResource,
                      public IDestroyListener
{
    IRESOURCE(ShaderProgram)

public:
    bool Load(const Path &vshaderPath, const Path &fshaderPath);
    bool Load(Shader* vShader, Shader* fShader);

    bool Link();
    bool IsLinked() const;

    void Bind() const override;
    void UnBind() const override;
    GL::BindTarget GetGLBindTarget() const override;

    template<class T, class=TT_NOT_POINTER(T)>
    bool Set(const String &name, const T &v)
    {
        ASSERT(GL::IsBound(this));
        int location = GetUniformLocation(name);
        if (location >= 0) { GL::Uniform(location, v); }
        return (location >= 0);
    }

    bool Set(const String &name, Texture2D *texture);

    bool Refresh();
    bool SetVertexShader(Shader* vertexShader);
    bool SetFragmentShader(Shader* fragmentShader);

    Shader* GetVertexShader() const;
    Shader* GetFragmentShader() const;

    GLint GetUniformLocation(const String &name) const;

    // IDestroyListener
    void OnDestroyed(Object *obj) override;

private:
    RH<Shader> p_vshader;
    RH<Shader> p_fshader;
    bool m_isLinked = false;

    mutable std::unordered_map<String, GLuint> m_nameToLocationCache;
    mutable std::unordered_map<String, Texture2D*> m_namesToTexture;
    // mutable Map<String, GLuint> m_nameToLocationCache;
    // mutable Map<String, Texture2D*> m_namesToTexture;

    ShaderProgram();
    ShaderProgram(Shader *vShader, Shader *fShader);
    ShaderProgram(const Path& vShaderPath,
                  const Path& fShaderPath);
    virtual ~ShaderProgram();

    bool BindTextureToAvailableUnit(const String &texName, Texture2D *texture) const;
    void UpdateTextureBindings() const;
};

NAMESPACE_BANG_END

#endif // SHADERPROGRAM_H
