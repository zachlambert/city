#version 330 core

in vec4 color_vertex;
in vec3 normal_worldspace;

out vec4 color_frag;

void main()
{
    // Temporary: Make it brighter facing the camera
    float value = 0.5 * (1.0 + dot(normal_worldspace, vec3(0, 0, 1)));
    color_frag = color_vertex;
    color_frag.xyz = color_frag.xyz * value;
}
