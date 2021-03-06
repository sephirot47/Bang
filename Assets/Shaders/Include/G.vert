#define BANG_VERTEX
#define BANG_G
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;

out vec3 B_FIn_Position;
out vec3 B_FIn_Normal;
out vec2 B_FIn_Uv;

void main()
{
    B_FIn_Position = ( B_Model * vec4(B_VIn_Position, 1) ).xyz;
    B_FIn_Normal   = normalize( (B_Normal * vec4(B_VIn_Normal, 0)).xyz );
    B_FIn_Uv       = vec2(B_VIn_Uv.x, B_VIn_Uv.y) * B_UvMultiply + B_UvOffset;
    gl_Position    = B_PVM * vec4(B_VIn_Position, 1);
}
