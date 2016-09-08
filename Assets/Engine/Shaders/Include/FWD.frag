#include "Version.glsl"
#include "Uniforms.glsl"

struct B_VertexIn
{
    vec3 position_world;    // Passed from vert
    vec3 normal_world;      // Passed from vert
    vec2 uv;                // Passed from vert
    vec4 diffuseColor;      // Passed from GBuffer
    bool receivesLighting;  // Passed from GBuffer
    float shininess;        // Passed from GBuffer
    float depth;            // Passed from GBuffer
};

struct B_VertexOut
{
    vec4 color;
};

B_VertexIn B_vin;
B_VertexOut B_vout;

in vec3 B_position_world_vout_fin;
in vec3 B_normal_world_vout_fin;
in vec2 B_uv_vout_fin;

out vec4 B_position_fout_gin;
out vec4 B_normal_fout_gin;
out vec4 B_uv_fout_gin;
out vec4 B_diffuse_fout_gin;
out vec4 B_materialProps_fout_gin;
out vec4 B_depth_fout_gin;
out vec4 B_color_gout_gin; // Accumulated color

void InitMain()
{
    B_vin.position_world      = B_position_world_vout_fin;
    B_vin.normal_world        = normalize(B_normal_world_vout_fin);
    B_vin.uv                  = B_uv_vout_fin;

    // Default value
    B_vout.color              = B_material_diffuse_color;
}

void EndMain()
{
    B_color_gout_gin = B_vout.color;
}
