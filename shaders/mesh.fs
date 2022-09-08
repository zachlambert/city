#version 330 core

in vec4 color_vertex;
in vec3 normal_viewspace;

out vec4 color_frag;

void main()
{
    // Temporary: Make it brighter facing the camera
    color_frag = color_vertex * dot(normal_viewspace, vec3(0, 0, -1));
}
