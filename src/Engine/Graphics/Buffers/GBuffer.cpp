#include "Bang/GBuffer.h"

#include "Bang/GL.h"
#include "Bang/Math.h"
#include "Bang/Color.h"
#include "Bang/AARect.h"
#include "Bang/GEngine.h"
#include "Bang/Texture2D.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

GBuffer::GBuffer(int width, int height) : Framebuffer(width, height)
{
    Bind();
    CreateAttachment(AttColor,        GL::ColorFormat::RGBA_UByte8);
    CreateAttachment(AttDiffuse,      GL::ColorFormat::RGBA_UByte8);
    CreateAttachment(AttNormal,       GL::ColorFormat::RGB10_A2_UByte);
    CreateAttachment(AttMisc,         GL::ColorFormat::RGB10_A2_UByte);
    CreateAttachment(AttColorRead,    GL::ColorFormat::RGBA_UByte8);
    CreateAttachment(AttDepthStencil, GL::ColorFormat::Depth24_Stencil8);
    UnBind();
}

GBuffer::~GBuffer()
{
}

void GBuffer::BindAttachmentsForReading(ShaderProgram *sp)
{
    if (!sp) { return; }
    ASSERT(GL::IsBound(sp));

    sp->Set(GBuffer::GetNormalsTexName(), GetAttachmentTexture(AttNormal), false);
    sp->Set(GBuffer::GetDiffuseTexName(), GetAttachmentTexture(AttDiffuse), false);
    sp->Set(GBuffer::GetMiscTexName(), GetAttachmentTexture(AttMisc), false);
    sp->Set(GBuffer::GetColorsTexName(), GetAttachmentTexture(AttColorRead), false);
    sp->Set(GBuffer::GetDepthStencilTexName(), GetAttachmentTexture(AttDepthStencil), false);
}


void GBuffer::ApplyPass(ShaderProgram *sp,
                        bool willReadFromColor,
                        const AARect &mask)
{
    if (!sp) { return; }
    ASSERT(GL::IsBound(this));
    ASSERT(GL::IsBound(sp));

    // Save state
    GL::StencilOperation prevStencilOp = GL::GetStencilOp();
    PushDrawAttachments();

    // Set state
    GL::SetStencilOp(GL::StencilOperation::Keep); // Dont modify stencil
    if (willReadFromColor) { PrepareColorReadBuffer(mask); }
    BindAttachmentsForReading(sp);
    SetColorDrawBuffer();

    GEngine::GetActive()->RenderViewportRect(sp, mask);

    // Restore state
    PopDrawAttachments();
    GL::SetStencilOp(prevStencilOp);
}

void GBuffer::PrepareColorReadBuffer(const AARect &readNDCRect)
{
    PushDrawAttachments();
    SetReadBuffer(AttColor);
    SetDrawBuffers({AttColorRead});
    AARect rf (readNDCRect * 0.5f + 0.5f);
    AARecti r ( AARect(Vector2::Floor(rf.GetMin()),
                   Vector2::Ceil(rf.GetMax())) * GetSize() );
    GL::BlitFramebuffer(r, r, GL::FilterMode::Nearest,
                        GL::BufferBit::Color);
    PopDrawAttachments();
}

void GBuffer::SetAllDrawBuffers() const
{
    SetDrawBuffers({GBuffer::AttColor,  GBuffer::AttDiffuse,
                    GBuffer::AttNormal, GBuffer::AttMisc
                   });
}

void GBuffer::SetAllDrawBuffersExceptColor()
{
    SetDrawBuffers({GBuffer::AttDiffuse, GBuffer::AttNormal, GBuffer::AttMisc});
}

void GBuffer::SetColorDrawBuffer()
{
    SetDrawBuffers({GBuffer::AttColor});
}

void GBuffer::ClearAllBuffersExceptColor()
{
    GL::ClearStencilBuffer(0);
    ClearDepth(1.0f);

    SetDrawBuffers({GBuffer::AttNormal, GBuffer::AttDiffuse, GBuffer::AttMisc});
    GL::ClearColorBuffer(Color::Zero);
}

void GBuffer::ClearBuffersAndBackground(const Color &backgroundColor)
{
    ClearAllBuffersExceptColor();
    SetDrawBuffers({GBuffer::AttColor}); // , GBuffer::AttColorRead});
    GL::ClearColorBuffer(backgroundColor);
}

String GBuffer::GetMiscTexName() { return "B_GTex_Misc"; }
String GBuffer::GetColorsTexName() { return "B_GTex_Color"; }
String GBuffer::GetNormalsTexName() { return "B_GTex_Normal"; }
String GBuffer::GetDiffuseTexName() { return "B_GTex_DiffColor"; }
String GBuffer::GetDepthStencilTexName() { return "B_GTex_DepthStencil"; }
