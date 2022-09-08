#version 330 core

layout (location = 0) in vec2 pos_modelspace;
layout (location = 1) in vec3 normal_modelspace;
layout (location = 2) in vec4 color;
layout (location = 3) in mat4 M;

uniform mat4 V;

out vec4 color_vertex;
out vec3 normal_viewspace;

void main()
{
    color_vertex = color;
    gl_Position = vec4(V * M * pos_modelspace, 1).xyz;
    normal_viewspace = vec4(V * M * normal_modelspace, 0).xyz;
}
