#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <functional>

#include "Bang/Font.h"
#include "Bang/Rect.h"
#include "Bang/TextFormat.h"

FORWARD   class Font;

class TextFormatter
{
public:
    struct CharRect
    {
        Recti rect;
        char character;
        CharRect(char _c, const Recti &_rect) :
            rect(_rect), character(_c) {}
    };

    static Array<CharRect> GetFormattedTextPositions(
                                            const String &content,
                                            const Font *font,
                                            HorizontalAlignment hAlignment,
                                            VerticalAlignment vAlignment,
                                            bool wrapping,
                                            int textSize,
                                            const Vector2i &spacing,
                                            const Recti &limitsRect);
    TextFormatter() = delete;

private:
    static Array< Array<CharRect> > SplitCharRectsInLines(
                                const String &content,
                                const Font *font,
                                const Array<CharRect> &charRects,
                                const Recti &limitsRect,
                                const Vector2i &spacing,
                                int textSize,
                                bool wrapping);

    static void ApplyAlignment(Array< Array<CharRect> > *linedCharRects,
                               HorizontalAlignment hAlignment,
                               VerticalAlignment vAlignment,
                               const Recti &limitsRect);

    static Recti GetCharRect(const Font *font,
                             int textSize,
                             char c);
    static int GetCharAdvanceX(const Font *font,
                               int textSize,
                               const String &content,
                               int currentCharIndex);
};

#endif // TEXTFORMATTER_H
