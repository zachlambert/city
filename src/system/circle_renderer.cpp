
#include "asset/shader.h"
#include "system/circle_renderer.h"


CircleRenderer::CircleRenderer(
    const WindowState& window_state,
    const CircleList& circles,
    const Args& args
):
    window_state(window_state),
    circles(circles)
{
    program_id = load_shader(
        args.shader_path + "circle.vs",
        args.shader_path + "circle.fs"
    );

    v_loc = glGetUniformLocation(program_id, "V");

    // Quad data

    quad_vertices[0] = { -1, 1 };
    quad_vertices[1] = { 1, -1 };
    quad_vertices[2] = { -1, -1 };
    quad_vertices[3] = { -1, 1 };
    quad_vertices[4] = { 1, -1 };
    quad_vertices[5] = { 1, 1 };

    // VAO

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Quad buffer

    glGenBuffers(1, &quad_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, quad_VBO);
    {
        glBufferData(
            GL_ARRAY_BUFFER,
            quad_vertices.size() * sizeof(glm::vec2),
            &quad_vertices[0],
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
            (void*)0
        );
        glEnableVertexAttribArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //  Instances buffer

    glGenBuffers(1, &instance_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    {
        // Data is dynamic, load each update

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, sizeof(Circle),
            (void*)offsetof(Circle, pos)
        );
        glVertexAttribDivisor(1, 1);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 4, GL_FLOAT, GL_FALSE, sizeof(Circle),
            (void*)offsetof(Circle, color)
        );
        glVertexAttribDivisor(2, 1);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3, 1, GL_FLOAT, GL_FALSE, sizeof(Circle),
            (void*)offsetof(Circle, radius)
        );
        glVertexAttribDivisor(3, 1);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
            4, 1, GL_FLOAT, GL_FALSE, sizeof(Circle),
            (void*)offsetof(Circle, edge_width)
        );
        glVertexAttribDivisor(4, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CircleRenderer::tick()
{
    glUseProgram(program_id);
    glBindVertexArray(VAO);
    glUniformMatrix4fv(v_loc, 1, GL_FALSE, &window_state.view_matrix[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Circle) * circles.size(),
        &circles[0],
        GL_STREAM_DRAW
    );

    glDrawArraysInstanced(
        GL_TRIANGLES,
        0, quad_vertices.size(),
        circles.size()
    );
}
