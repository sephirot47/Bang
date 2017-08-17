#ifndef UITEXTRENDERER_H
#define UITEXTRENDERER_H

#include "Bang/Mesh.h"
#include "Bang/Rect.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/TextFormat.h"
#include "Bang/UIRenderer.h"

FORWARD class Font;

class UITextRenderer : public UIRenderer
{
    COMPONENT(UITextRenderer)

public:
    UITextRenderer();
    virtual ~UITextRenderer();

    // UIRenderer
    virtual void OnRender() override;
    virtual void OnUpdate() override;
    virtual void OnParentSizeChanged() override;

    // UIRenderer
    virtual void Bind() const override;
    virtual void UnBind() const override;

    void RefreshMesh();

    void SetFont (Font *font);
    void SetTextColor(const Color &textColor);
    void SetHorizontalAlign(HorizontalAlignment horizontalAlignment);
    void SetVerticalAlign(VerticalAlignment verticalAlignment);
    void SetKerning(bool kerning);
    void SetWrapping(bool wrapping);
    void SetContent(const String &content);
    void SetTextSize(int size);
    void SetSpacing(const Vector2i &spacing);

    Font* GetFont() const;
    bool GetKerning() const;
    const Color& GetTextColor() const;
    bool GetWrapping() const;
    VerticalAlignment GetVerticalAlignment() const;
    HorizontalAlignment GetHorizontalAlignment() const;
    const String& GetContent() const;
    int GetTextSize() const;
    Vector2i GetSpacing() const;
    const Array<Rect>& GetCharRectsNDC() const;
    const Rect& GetCharRectNDC(uint charIndex) const;
    Rect GetContentNDCRect() const;
    virtual Rect GetBoundingRect(Camera *camera = nullptr) const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // SerializableObject
    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;

protected:
    String m_content       = "";
    Font *m_font           = nullptr;
    int m_textSize         = 64;
    Vector2i m_spacing     = Vector2i(5, 200);
    bool m_kerning         = false;
    Rect m_textRectNDC     = Rect::Zero;

    bool m_hasChanged      = false;

    bool m_wrapping = false;
    HorizontalAlignment m_horizontalAlignment = HorizontalAlignment::Left;
    VerticalAlignment m_verticalAlignment     = VerticalAlignment::Top;

    Array<Rect> m_charRectsLocalNDC;

private:
    Mesh *m_mesh = nullptr;
};

#endif // UITEXTRENDERER_H
