#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include "state/viewport.h"
#include "world/world.h"
#include "asset/mesh.h"


class TerrainRenderer {
public:
    struct Args {
        std::string shader_path;
    };
    TerrainRenderer(
        const Viewport& viewport,
        const World& world,
        const Args& args);
    void tick();

    void set_region(int region);

    void load_mesh(
        const std::vector<MeshVertex>& mesh_vertices,
        const std::vector<unsigned short>& mesh_indices);
    void clear();

    void update_buffers();

private:
    struct RegionData {
        size_t index_offset;
        size_t index_count;
        RegionData(size_t index_offset):
            index_offset(index_offset), index_count(0)
        {}
    };

    int selected_region;
    std::unordered_map<int, RegionData> region_datas;

    static constexpr size_t default_vbo_capacity = 128;
    size_t vbo_size;
    size_t vbo_capacity;
    std::vector<MeshVertex> vertices;
    static constexpr size_t default_ebo_capacity = 128;
    size_t ebo_size;
    size_t ebo_capacity;
    std::vector<unsigned short> indices;

    const Viewport& viewport;
    const World& world;

    unsigned int VAO;
    unsigned int vertex_VBO;
    unsigned int vertex_EBO;

    unsigned int program_id;
    unsigned int pv_loc;
};
