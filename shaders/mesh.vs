#version 330 core

layout (location = 0) in vec3 pos_modelspace;
layout (location = 1) in vec3 normal_modelspace;
layout (location = 2) in vec4 color;
layout (location = 3) in mat4 M;

uniform mat4 PV;

out vec4 color_vertex;
out vec3 normal_worldspace;

void main()
{
    color_vertex = color;
    gl_Position = PV * M * vec4(pos_modelspace, 1);
    normal_worldspace = (M * vec4(pos_modelspace, 1)).xyz;
}
