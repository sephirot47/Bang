#include "Bang/G_FontSheetCreator.h"

#include "Bang/Math.h"
#include "Bang/Array.h"
#include "Bang/Debug.h"
#include "Bang/Vector2.h"
#include "Bang/G_Texture2D.h"

#include <ftglyph.h>

G_FontSheetCreator *G_FontSheetCreator::m_singleton = nullptr;

G_FontSheetCreator::G_FontSheetCreator()
{
    FT_Error error = FT_Init_FreeType( &m_ftLibrary );
    if (error)
    {
        Debug_Error("Could not init FreeType library: Error(" << error <<  ")");
    }
}

G_FontSheetCreator::~G_FontSheetCreator()
{
}

bool G_FontSheetCreator::Init()
{
    if (G_FontSheetCreator::m_singleton) { return true; }

    G_FontSheetCreator::m_singleton = new G_FontSheetCreator();
    return true;
}

int G_FontSheetCreator::GetGlyphIndex(FT_Face face, char c)
{
    int glyph_index = FT_Get_Char_Index(face, c);
    return glyph_index;
}

bool G_FontSheetCreator::LoadAtlasTexture(
                         const Path &fontFilepath,
                         int glyphSizePx,
                         G_Texture2D **atlasTexture,
                         Map<char, std::pair<Vector2, Vector2> > *charAtlasUvs,
                         Map<char, G_Font::CharGlyphMetrics> *resultMetrics,
                         FT_Face *fontFace)
{
    if (!G_FontSheetCreator::Init()) { return false; }

    charAtlasUvs->Clear();
    resultMetrics->Clear();

    FT_Face face;
    int error = FT_New_Face(m_singleton->m_ftLibrary,
                            fontFilepath.GetAbsolute().ToCString(),
                            0, &face);
    if (error)
    {
        Debug_Error("Failed to load font '" << fontFilepath << "': Error("
                    << error << ")");
        return false;
    }

    // Set the pixel size ( rasterize (?) )
    error = FT_Set_Pixel_Sizes(face, glyphSizePx, glyphSizePx);
    if (error)
    {
        Debug_Error("Failed to set font pixel size of '" << fontFilepath
                    << "': Error(" << error << ")");
        return false;
    }

    String charactersToLoadStr = "";
    charactersToLoadStr += "abcdefghijklmnopqrstuvwxyz";
    charactersToLoadStr += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    charactersToLoadStr += "0123456789()[]{}*.,;:-_=!<>+";
    charactersToLoadStr += "/\\$%&@\"'#¿?^";

    // Generate the atlas, adding each char in a simple grid
    uint numChars        = charactersToLoadStr.Length();
    uint charsPerRowCol  = Math::Sqrt(numChars) + 1;
    uint charInAtlasSize = G_Font::c_charLoadSize * 2;
    uint sideSize        = charsPerRowCol * charInAtlasSize;
    G_Image atlasImage(sideSize, sideSize);
    for (int i = 0; i < numChars; ++i)
    {
        const char c = charactersToLoadStr[i];

        error = FT_Load_Glyph(face, G_FontSheetCreator::GetGlyphIndex(face, c),
                              FT_LOAD_DEFAULT);
        if (error)
        {
            Debug_Error("Failed to load glyph '" << c <<
                        "': Error(" << error << ")");
            continue;
        }

        // Move the face's glyph into a glyph object.
        FT_Glyph glyph;
        error = FT_Get_Glyph( face->glyph, &glyph );
        if (error)
        {
            Debug_Error("Failed to get glyph '" << c << "' for font '"
                        << fontFilepath << "': Error(" << error << ")");
            continue;
        }


        // Convert the glyph to a bitmap.
        FT_Vector  origin; origin.x = origin.y = 0;
        FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, &origin, true);
        FT_BitmapGlyph bitmap_glyph = FT_BitmapGlyph(glyph);
        FT_Bitmap &bitmap = bitmap_glyph->bitmap;


        // Get some metrics (they are given in 1/64 pixels...)
        // These are measurements relative to the full tex quad (size x size)
        G_Font::CharGlyphMetrics charMetrics;
        charMetrics.width    =  face->glyph->metrics.width  / 64;
        charMetrics.height   =  face->glyph->metrics.height / 64;
        charMetrics.bearingX = (face->glyph->metrics.horiBearingX) / 64;
        charMetrics.bearingY = (face->glyph->metrics.horiBearingY) / 64;
        charMetrics.advance  = (face->glyph->metrics.horiAdvance)  / 64;
        charMetrics.originY  =  origin.y / 64;
        resultMetrics->Set(c, charMetrics);

        const uint charRow = i / charsPerRowCol;
        const uint charCol = i % charsPerRowCol;

        Vector2 minPixel(Math::Infinity<float>());
        Vector2 maxPixel(Math::NegativeInfinity<float>());
        if (charMetrics.width > 0 && charMetrics.height > 0)
        {
            const uint offX = charInAtlasSize * charCol;
            const uint offY = charInAtlasSize * charRow;
            for(int y = 0; y < charMetrics.height; y++)
            {
                for(int x = 0; x < charMetrics.width; x++)
                {
                    float pixelAlpha = bitmap.buffer[y * charMetrics.width + x];
                    Color pxColor = Color(1.0f, 1.0f, 1.0f,
                                          pixelAlpha / 255.0f);
                    minPixel.x = Math::Min(minPixel.x, float(offX + x));
                    minPixel.y = Math::Min(minPixel.y, float(offY + y));
                    maxPixel.x = Math::Max(maxPixel.x, float(offX + x));
                    maxPixel.y = Math::Max(maxPixel.y, float(offY + y));
                    atlasImage.SetPixel(offX + x, offY + y, pxColor);
                }
            }
        }

        Vector2 uvMin = minPixel / atlasImage.GetSize();
        Vector2 uvMax = maxPixel / atlasImage.GetSize();
        uvMin.y       = 1.0 - uvMin.y;
        uvMax.y       = 1.0 - uvMax.y;
        charAtlasUvs->Set(c, std::make_pair(uvMin, uvMax) );

        FT_Done_Glyph(glyph);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    *atlasTexture = new G_Texture2D();
    (*atlasTexture)->LoadFromImage(atlasImage);
    (*atlasTexture)->SetWrapMode(G_Texture::WrapMode::ClampToEdge);
    (*atlasTexture)->SetFilterMode(G_Texture::FilterMode::Trilinear);
    (*atlasTexture)->SetAlphaCutoff(0.0f);
    (*atlasTexture)->GenerateMipMaps();

    *fontFace = face;
    FT_Done_Face(face);
    return true;
}
