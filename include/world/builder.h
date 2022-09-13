#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <stack>
#include <yaml-cpp/yaml.h>
#include "world/world.h"
#include "system/terrain_renderer.h"
#include "maths/geometry.h"
#include "maths/angles.h"
#include "maths/random.h"
#include "maths/collision.h"


class Builder {
    struct Node {
        glm::vec3 pos;
        std::vector<int> edges;
        AngleSet angle_set;
        int iteration;
        int region;
        Node(const glm::vec3& pos, int iteration):
            pos(pos),
            angle_set(),
            iteration(iteration),
            region(-1)
        {}
    };

    struct Edge {
        int node_1;
        int node_2;
        Capsule capsule;
    };

public:
    Builder(YAML::Node config);
    bool add_road();
    void clear();
    void write_world(World& world, TerrainRenderer& terrain_renderer);

private:
    struct EdgeQuery {
        int edge_i;
        bool collision;
        float distance;
        glm::vec3 snap_pos;
    };
    EdgeQuery query_edge(int node_from, const glm::vec3& to);

    void append_angle_set_mesh(
        std::vector<MeshVertex>& vertices,
        std::vector<unsigned short>& indices,
        const AngleSet& angle_set,
        const glm::vec3& pos,
        const glm::vec4& color,
        float radius,
        float resolution);

    float angle_spacing;
    float min_length;
    float max_length;
    float road_width;
    int max_iteration;
    glm::vec4 road_color;

    float snap_distance = 5; // TODO: Set in config
    float node_snap_distance = 5;
    float edge_snap_angle = 0.5;

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<int> growable;
};
