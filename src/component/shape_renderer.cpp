
#include "state.h"
#include "shader.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


void State::create_shape_renderer()
{
    assert(!shape_renderer.valid);

    shape_renderer.program_id = load_shader(
        window.shader_path + "circle.vs",
        window.shader_path + "circle.fs"
    );

    shape_renderer.v_loc = glGetUniformLocation(shape_renderer.program_id, "V");

    // Quad data

    shape_renderer.quad_vertices[0] = { -1, 1 };
    shape_renderer.quad_vertices[1] = { 1, -1 };
    shape_renderer.quad_vertices[2] = { -1, -1 };
    shape_renderer.quad_vertices[3] = { -1, 1 };
    shape_renderer.quad_vertices[4] = { 1, -1 };
    shape_renderer.quad_vertices[5] = { 1, 1 };

    // VAO

    glGenVertexArrays(1, &shape_renderer.VAO);
    glBindVertexArray(shape_renderer.VAO);

    // Quad buffer

    glGenBuffers(1, &shape_renderer.quad_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, shape_renderer.quad_VBO);
    {
        glBufferData(
            GL_ARRAY_BUFFER,
            shape_renderer.quad_vertices.size() * sizeof(glm::vec2),
            &shape_renderer.quad_vertices[0],
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

    glGenBuffers(1, &shape_renderer.instance_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, shape_renderer.instance_VBO);
    {
        // Data is dynamic, load each update

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeRenderer::CircleInstance),
            (void*)offsetof(ShapeRenderer::CircleInstance, pos)
        );
        glVertexAttribDivisor(1, 1);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 4, GL_FLOAT, GL_FALSE, sizeof(ShapeRenderer::CircleInstance),
            (void*)offsetof(ShapeRenderer::CircleInstance, color)
        );
        glVertexAttribDivisor(2, 1);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3, 1, GL_FLOAT, GL_FALSE, sizeof(ShapeRenderer::CircleInstance),
            (void*)offsetof(ShapeRenderer::CircleInstance, radius)
        );
        glVertexAttribDivisor(3, 1);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
            4, 1, GL_FLOAT, GL_FALSE, sizeof(ShapeRenderer::CircleInstance),
            (void*)offsetof(ShapeRenderer::CircleInstance, edge_width)
        );
        glVertexAttribDivisor(4, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shape_renderer.valid = true;
}

void State::remove_shape_renderer()
{

}

void State::update_shape_renderer()
{
    glUseProgram(shape_renderer.program_id);
    glBindVertexArray(shape_renderer.VAO);
    glUniformMatrix4fv(shape_renderer.v_loc, 1, GL_FALSE, &window.view_matrix[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, shape_renderer.instance_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(ShapeRenderer::CircleInstance) * shape_renderer.circle_instances.size(),
        &shape_renderer.circle_instances[0],
        GL_STREAM_DRAW
    );

    glDrawArraysInstanced(
        GL_TRIANGLES,
        0, shape_renderer.quad_vertices.size(),
        shape_renderer.circle_instances.size()
    );
}
