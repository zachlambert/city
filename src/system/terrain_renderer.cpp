
#include "asset/shader.h"
#include "system/terrain_renderer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


TerrainRenderer::TerrainRenderer(
    const Viewport& viewport,
    const World& world,
    const Args& args
):
    viewport(viewport),
    world(world)
{
    program_id = load_shader(
        args.shader_path + "terrain.vs",
        args.shader_path + "terrain.fs"
    );

    pv_loc = glGetUniformLocation(program_id, "PV");

    // VAO

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex bufer

    glGenBuffers(1, &vertex_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    {
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(MeshVertex),
            &vertices[0],
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
            (void*)offsetof(MeshVertex, pos)
        );
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
            (void*)offsetof(MeshVertex, normal)
        );
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(
            2, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
            (void*)offsetof(MeshVertex, color)
        );
        glEnableVertexAttribArray(2);
    }

    // Element buffer
    
    glGenBuffers(1, &vertex_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned short),
        &indices[0],
        GL_STATIC_DRAW
    );

    selected_region = false;
}

void TerrainRenderer::set_region(int region)
{
    auto iter = region_datas.find(region);
    assert(iter == region_datas.end());

    region_datas.emplace(region, RegionData(indices.size()));
    selected_region = region;
}

void TerrainRenderer::load_mesh(
    const std::vector<MeshVertex>& mesh_vertices,
    const std::vector<unsigned short>& mesh_indices)
{
    auto iter = region_datas.find(selected_region);
    assert(iter != region_datas.end());

    RegionData& region_data = iter->second;
    region_data.index_count += mesh_indices.size();

    size_t vertices_start = vertices.size();
    size_t indices_start = indices.size();
    std::copy(mesh_vertices.begin(), mesh_vertices.end(), std::back_inserter(vertices));
    std::copy(mesh_indices.begin(), mesh_indices.end(), std::back_inserter(indices));
    for (size_t i = indices_start; i < indices.size(); i++) {
        indices[i] += vertices_start;
    }
}

void TerrainRenderer::update_buffers()
{
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(MeshVertex),
        &vertices[0],
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned short),
        &indices[0],
        GL_STATIC_DRAW
    );
}

void TerrainRenderer::tick()
{
    glm::mat4 pv_matrix = viewport.projection_matrix * viewport.view_matrix;

    glUseProgram(program_id);
    glBindVertexArray(VAO);
    glUniformMatrix4fv(pv_loc, 1, GL_FALSE, &pv_matrix[0][0]);

    for (const auto& iter: region_datas) {
        const RegionData& region_data = iter.second;
        glDrawElements(
            GL_TRIANGLES,
            region_data.index_count,
            GL_UNSIGNED_SHORT,
            (void*)region_data.index_offset
        );
    }
}
