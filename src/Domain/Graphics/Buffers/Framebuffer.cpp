#include "Bang/Framebuffer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Bang/stb_image_write.h"

#include "Bang/GL.h"
#include "Bang/Math.h"
#include "Bang/Debug.h"
#include "Bang/Vector2.h"
#include "Bang/RenderTexture.h"

Framebuffer::Framebuffer(int width, int height) : m_width(width),
                                                  m_height(height)
{
    glGenFramebuffers(1, &m_idGL);
}

Framebuffer::~Framebuffer()
{
    if (m_depthRenderBufferId != 0)
    {
        glDeleteRenderbuffers(1, &m_depthRenderBufferId);
    }
    glDeleteFramebuffers(1, &m_idGL);
}

void Framebuffer::CreateColorAttachment(AttachmentId attId)
{
    RenderTexture *tex = new RenderTexture();
    tex->CreateEmpty(GetWidth(), GetHeight());

    m_colorAttachmentIds.PushBack(attId);
    m_attachmentId_To_Texture.Set(attId, tex);

    Bind();
    GL::ClearError();
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attId, GL_TEXTURE_2D,
                           tex->GetGLId(), 0);
    GL_CheckError();
    GL::CheckFramebufferError();
    UnBind();
    //
}

void Framebuffer::CreateDepthRenderbufferAttachment()
{
    Bind();
    GL::ClearError();
    glGenRenderbuffers(1, &m_depthRenderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                          m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, m_depthRenderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GL_CheckError();
    GL::CheckFramebufferError();
    UnBind();
}

RenderTexture *Framebuffer::GetAttachmentTexture(AttachmentId attId) const
{
    if (!m_attachmentId_To_Texture.ContainsKey(attId)) { return nullptr; }
    return m_attachmentId_To_Texture.Get(attId);
}

void Framebuffer::SetAllDrawBuffers() const
{
    SetDrawBuffers(m_colorAttachmentIds);
}

void Framebuffer::SetDrawBuffers(const Array<AttachmentId> &attIds) const
{
    GL::ClearError();
    glDrawBuffers(attIds.Size(), (const GLenum*)(&attIds[0]));
    GL_CheckError();
}

void Framebuffer::SetReadBuffer(AttachmentId attId) const
{
    GL::ClearError();
    glReadBuffer(attId);
    GL_CheckError();
}

Color Framebuffer::ReadColor(int x, int y, AttachmentId attId) const
{
    Bind();
    RenderTexture *t = GetAttachmentTexture(attId);
    SetReadBuffer(attId);
    Color readColor;
    glReadPixels(x, y, 1, 1, t->GetGLFormat(), t->GetGLDataType(), &readColor);
    UnBind();
    return readColor;
}


void Framebuffer::Resize(int width, int height)
{
    m_width  = Math::Max(width,  1);
    m_height = Math::Max(height, 1);

    for (auto it : m_attachmentId_To_Texture)
    {
        RenderTexture *t = it.second;
        if (t)
        {
            t->Resize(m_width, m_height);
        }
    }

    if (m_depthRenderBufferId != 0)
    {
        //TODO:  respect former bindings of renderbuffers
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                              m_width, m_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

int Framebuffer::GetWidth() const
{
    return m_width;
}

int Framebuffer::GetHeight() const
{
    return m_height;
}

Vector2 Framebuffer::GetSize() const
{
    return Vector2(GetWidth(), GetHeight());
}

void Framebuffer::Clear()
{
    SetAllDrawBuffers();
    GL::ClearDepthBuffer();
    GL::ClearColorBuffer();
}

void Framebuffer::ClearDepth(float clearDepth)
{
    GL::ClearDepthBuffer(clearDepth);
}

void Framebuffer::ClearColor(const Color &clearColor)
{
    SetAllDrawBuffers();
    GL::ClearColorBuffer(clearColor);
}

GL::BindTarget Framebuffer::GetGLBindTarget() const
{
    return GL::BindTarget::Framebuffer;
}

void Framebuffer::Bind() const
{
    GL::Bind(this);
    GL::SetViewport(0, 0, m_width, m_height);
}
void Framebuffer::UnBind() const
{
    GL::UnBind(this);
}

void Framebuffer::SaveToImage(AttachmentId attId, const String &filepath,
                              bool invertY) const
{
    Bind();
    Texture *tex = m_attachmentId_To_Texture.Get(attId);
    if (tex)
    {
        SetReadBuffer(attId);
        unsigned int bytesSize = m_width * m_height * 4;
        unsigned char *pixels = new unsigned char[bytesSize];
        glReadPixels(0, 0, m_width, m_height,
                     GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        if (invertY)
        {
            unsigned char *pixelsCpy = new unsigned char[bytesSize];
            memcpy(pixelsCpy, pixels, bytesSize);
            for (int i = 0; i < m_width; ++i)
            {
                for (int j = 0; j < m_height; ++j)
                {
                    int coords    = j * m_width + i;
                    int invCoords = (m_height - j - 1) * m_width + i;
                    coords *= 4; invCoords *= 4;
                    pixels[coords + 0] = pixelsCpy[invCoords + 0];
                    pixels[coords + 1] = pixelsCpy[invCoords + 1];
                    pixels[coords + 2] = pixelsCpy[invCoords + 2];
                    pixels[coords + 3] = pixelsCpy[invCoords + 3];
                }
            }
            delete[] pixelsCpy;
        }
        stbi_write_bmp(filepath.ToCString(), m_width, m_height, 4, pixels);
        delete[] pixels;
    }
    UnBind();
}




