#version 330 core

layout (location = 0) in vec3 pos_worldspace;
layout (location = 1) in vec3 normal_worldspace_in;
layout (location = 2) in vec4 color;

uniform mat4 PV;

out vec4 color_vertex;
out vec3 normal_worldspace;

void main()
{
    color_vertex = color;
    gl_Position = PV * vec4(pos_worldspace, 1);
    normal_worldspace = normal_worldspace_in;
}
