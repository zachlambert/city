#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <string>

#include "system/window.h"
#include "component/mesh.h"
#include "asset/mesh.h"


class MeshRenderer {
public:
    struct Args {
        std::string shader_path;
    };
    MeshRenderer(
        const WindowState& window_state,
        const ComponentList<Mesh>& meshes,
        const Args& args);
    void tick();

    int get_mesh(const std::string& name);
    int load_mesh(
        const std::string& name,
        const std::vector<MeshVertex>& mesh_vertices,
        const std::vector<unsigned short>& mesh_indices);

private:
    struct MeshData {
        size_t index_offset;
        size_t index_count;
        size_t instance_count;
        size_t instance_next;
    };
    struct Instance {
        glm::mat4 model;
    };

    std::unordered_map<std::string, int> mesh_ids;
    std::vector<MeshData> datas;

    std::vector<MeshVertex> vertices;
    std::vector<unsigned short> indices;
    std::vector<Instance> instances;

    const WindowState& window_state;
    const MeshList& meshes;

    unsigned int VAO;
    unsigned int vertex_VBO;
    unsigned int vertex_EBO;
    unsigned int instance_VBO;

    unsigned int program_id;
    unsigned int pv_loc;
};
