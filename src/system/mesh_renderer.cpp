
#include "asset/shader.h"
#include "system/mesh_renderer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


MeshRenderer::MeshRenderer(
    const Viewport& viewport,
    const MeshList& meshes,
    const Args& args
):
    viewport(viewport),
    meshes(meshes)
{
    program_id = load_shader(
        args.shader_path + "mesh.vs",
        args.shader_path + "mesh.fs"
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

    //  Instance buffer

    glGenBuffers(1, &instance_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    {
        glBufferData(
            GL_ARRAY_BUFFER,
            instances.size() * sizeof(Instance),
            &instances[0],
            GL_STATIC_DRAW
        );

        for (size_t i = 0; i < 4; i++) {
            int index = 3 + i;
            glVertexAttribPointer(
                index, 4, GL_FLOAT, GL_FALSE, sizeof(Instance),
                (void*)(offsetof(Instance, model) + i * sizeof(glm::vec4))
            );
            glEnableVertexAttribArray(index);
            glVertexAttribDivisor(index, 1);
        }
    }
}

int MeshRenderer::get_mesh(const std::string& name)
{
    auto iter = mesh_ids.find(name);
    if (iter == mesh_ids.end()) return -1;
    return iter->second;
}

int MeshRenderer::load_mesh(
    const std::string& name,
    const std::vector<MeshVertex>& mesh_vertices,
    const std::vector<unsigned short>& mesh_indices)
{
    int existing = get_mesh(name);
    if (existing != -1) return existing;

    MeshData data;
    data.index_offset = indices.size();
    data.index_count = mesh_indices.size();
    data.instance_count = 0;

    size_t vertices_start = vertices.size();
    size_t indices_start = indices.size();
    std::copy(mesh_vertices.begin(), mesh_vertices.end(), std::back_inserter(vertices));
    std::copy(mesh_indices.begin(), mesh_indices.end(), std::back_inserter(indices));
    for (size_t i = indices_start; i < indices.size(); i++) {
        indices[i] += vertices_start;
    }

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

    int id = datas.size();
    datas.push_back(data);
    mesh_ids.emplace(name, id);
    return id;
}

void MeshRenderer::tick()
{
    for (auto& data: datas) {
        data.instance_count = 0;
        data.instance_next = 0;
    }

    for (size_t i = 0; i < meshes.size(); i++) {
        const Mesh& mesh = meshes[i];
        if (!mesh.valid || mesh.mesh_id == -1) continue;
        datas[mesh.mesh_id].instance_count++;
    }

    size_t instance_count = 0;
    for (auto& data: datas) {
        data.instance_next = instance_count;
        instance_count += data.instance_count;
    }

    instances.resize(instance_count);
    for (size_t i = 0; i < meshes.size(); i++) {
        const Mesh& mesh = meshes[i];
        if (!mesh.valid || mesh.mesh_id == -1) continue;

        MeshData& data = datas[mesh.mesh_id];
        glm::mat4& model = instances[data.instance_next].model;
        data.instance_next++;

        model =
            mesh.pose.to_mat4()
            * glm::scale(glm::vec3(mesh.scale, mesh.scale, mesh.scale));
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        instances.size() * sizeof(Instance),
        &instances[0],
        GL_STREAM_DRAW
    );

    glm::mat4 pv_matrix = viewport.projection_matrix * viewport.view_matrix;

    glUseProgram(program_id);
    glBindVertexArray(VAO);
    glUniformMatrix4fv(pv_loc, 1, GL_FALSE, &pv_matrix[0][0]);

    for (auto& data: datas) {
        size_t instance_offset = data.instance_next - data.instance_count;
        glDrawElementsInstancedBaseInstance(
            GL_TRIANGLES,
            data.index_count,
            GL_UNSIGNED_SHORT,
            0,
            data.instance_count,
            instance_offset
        );
    }
}
