#version 330 core

in vec4 vertex_color;
in vec2 vertex_pos;

out vec4 frag_color;

void main()
{
    if (length(vertex_pos) > 1) {
        frag_color = vec4(0, 0, 0, 0);
    } else {
        frag_color = vertex_color;
    }
}
