#version 460 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec4 i_color;

layout (location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};
layout (location = 0) out vec4 v_color;

void main()
{
    v_color = i_color;
    gl_Position = vec4(i_position, 1.0f);
}