#version 330 core

layout (location = 0) in vec2 vertex_pos_in;
layout (location = 1) in vec3 pos;
layout (location = 2) in vec4 color;
layout (location = 3) in float radius;
layout (location = 4) in float edgeWidth;

uniform mat4 V;

out vec4 vertex_color;
out vec2 vertex_pos;

void main()
{
    vec2 scaled_pos = pos.xy + radius * vertex_pos_in;
    gl_Position = V * vec4(scaled_pos, 0.0, 1.0);
    vertex_color = vec4(color.xyz, color.w);
    vertex_pos = vertex_pos_in;
}
