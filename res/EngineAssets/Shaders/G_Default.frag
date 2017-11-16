#include "G.frag"

void Main()
{
    vec4 texColor = vec4(1);
    if (B_HasTexture)
    {
        texColor = texture2D(B_Texture0, B_FIn_Uv);
        if (texColor.a <= 1.0f) discard;
    }
    B_FOut.Diffuse = B_MaterialDiffuseColor * texColor;
}