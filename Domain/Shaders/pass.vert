#version 130

in vec3 v;

out vec3 v_o;

void main()
{
    v_o = v;
    gl_Position = vec4(v, 1.0);
}
