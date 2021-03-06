#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <functional>

#include "Bang/Font.h"
#include "Bang/AARect.h"
#include "Bang/Alignment.h"

NAMESPACE_BANG_BEGIN

FORWARD class Font;
FORWARD class RectTransform;

class TextFormatter
{
public:
    struct CharRect
    {
        AARectf rectPx;
        char character;
        CharRect(char _c, const AARectf &_rect) : rectPx(_rect), character(_c) {}
        friend std::ostream& operator<<(std::ostream &os,
                                        const TextFormatter::CharRect &cr);
    };

    static Array<CharRect> GetFormattedTextPositions(
                                        const String &content,
                                        const Font *font,
                                        int fontSize,
                                        const AARecti &limitsRect,
                                        const Vector2 &spacingMultiplier,
                                        HorizontalAlignment hAlignment,
                                        VerticalAlignment vAlignment,
                                        bool wrapping,
                                        uint *numberOfLines);

    static Vector2i GetTextSizeOneLined(const String &content,
                                        const Font *font,
                                        int fontSize,
                                        const Vector2 &spacingMultiplier);

    TextFormatter() = delete;

private:
    static Array< Array<CharRect> > SplitCharRectsInLines(
                                            const String &content,
                                            const Font *font,
                                            int fontSize,
                                            const AARecti &limitsRect,
                                            const Vector2 &spacingMultiplier,
                                            const Array<CharRect> &charRects,
                                            bool wrapping);

    static void ApplyAlignment(Array< Array<CharRect> > *linedCharRects,
                               const AARecti &limitsRect,
                               const Font *font,
                               int fontSize,
                               HorizontalAlignment hAlignment,
                               VerticalAlignment vAlignment);

    static AARectf GetCharRect(char c,
                             const Font *font,
                             int fontSize);
    static int GetCharAdvanceX(const String &content,
                               const Font *font,
                               int fontSize,
                               int currentCharIndex);
};

inline std::ostream& operator<<(std::ostream &os,
                                const TextFormatter::CharRect &cr)
{ os << "(" << cr.character << ", " << cr.rectPx << ")"; return os; }

NAMESPACE_BANG_END

#endif // TEXTFORMATTER_H
