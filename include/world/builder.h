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
    bool edge_valid(int node_from, const Capsule& capsule);
    void append_angle_set_mesh(
        std::vector<MeshVertex>& vertices,
        std::vector<unsigned short>& indices,
        const AngleSet& angle_set,
        const glm::vec3& pos,
        const glm::vec4& color,
        float radius,
        float resolution);

    float angle_spacing = M_PI * 2 / 3;
    float min_length = 20;
    float max_length = 50;
    float road_width = 5;
    int max_iteration = 10;
    glm::vec4 road_color = glm::vec4(0.4, 0.4, 0.4, 1);

    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<int> growable;
};
