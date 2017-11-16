#include "PP.frag"

void Main()
{
    vec4 col = B_SampleColor();

    vec3 luma = vec3(0.299, 0.587, 0.114);
    B_FOut.Color = vec4( vec3(dot(luma, col.rgb)), 1 );
}