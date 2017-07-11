#include "Bang/G_Screen.h"

#include "Bang/GL.h"

G_Screen::G_Screen()
{
}

G_Screen::~G_Screen()
{
}

void G_Screen::Render()
{
}

void G_Screen::OnInitialize()
{
    glewExperimental = GL_TRUE;
    glewInit();

    GL::Enable(GL_DEPTH_TEST);
    GL::Enable(GL_STENCIL_TEST);
    GL::Enable(GL_CULL_FACE);
}

void G_Screen::OnResize(int w, int h)
{
    GL::SetViewport(0, 0, w, h);
    m_width = w;
    m_height = h;
}

float G_Screen::GetAspectRatio() const
{
    return float(GetWidth()) / GetHeight();
}

int G_Screen::GetWidth() const
{
    return m_width;
}

Vector2 G_Screen::GetSize() const
{
    return Vector2(GetWidth(), GetHeight());
}

Vector2 G_Screen::GetPixelClipSize() const
{
    return 1.0f / GetSize();
}

int G_Screen::GetHeight() const
{
    return m_height;
}
