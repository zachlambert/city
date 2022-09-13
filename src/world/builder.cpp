

#include "world/builder.h"


Builder::Builder(YAML::Node config)
{
    angle_spacing = config["angle_spacing"].as<float>();
    min_length = config["min_length"].as<float>();
    max_length = config["max_length"].as<float>();
    road_width = config["road_width"].as<float>();
    max_iteration = config["max_iteration"].as<int>();
    auto road_color_arr = config["road_color"];
    for (size_t i = 0; i < road_color_arr.size(); i++) {
        road_color[i] = road_color_arr[i].as<float>();
    }

    Node root(glm::vec3(0, 0, 0), 0);
    nodes.emplace_back(root);
    growable.push_back(0);
}

bool Builder::add_road()
{
    while (true) {
        if (growable.empty()) {
            return false;
        }

        size_t growable_index = rand() % growable.size();
        int node_i = growable[growable_index];
        Node& node = nodes[node_i];

        if (node.angle_set.empty()) {
            growable[growable_index] = growable.back();
            growable.pop_back();
            continue;
        }

        float angle = node.angle_set.get(rand_float(), angle_spacing/2);
        AngleInterval interval(angle, angle_spacing);
        AngleInterval opp_interval(clamp_angle(angle + M_PI), angle_spacing);

        node.angle_set.remove(interval);
        node.angle_set.cull(angle_spacing);

        float length = min_length + rand_float() * (max_length - min_length);
        glm::vec3 next_pos = node.pos + length * glm::vec3(std::cos(angle), std::sin(angle), 0);

        Capsule capsule(node.pos, next_pos, road_width/2.f);
        if (!edge_valid(node_i, capsule)) {
            continue;
        }

        int next_i = nodes.size();

        Edge edge;
        edge.node_1 = node_i;
        edge.node_2 = next_i;
        edge.capsule = capsule;
        int edge_i = edges.size();
        edges.push_back(edge);

        node.edges.push_back(edge_i);

        Node next(next_pos, node.iteration + 1);
        next.edges.push_back(edge_i);
        next.angle_set.remove(opp_interval);
        next.angle_set.cull(angle_spacing);
        nodes.push_back(next);

        if (next.iteration < max_iteration) {
            growable.push_back(next_i);
        }
        return true;
    }
}

void Builder::clear()
{
    nodes.clear();
    edges.clear();
    growable.clear();

    Node root(glm::vec3(0, 0, 0), 0);
    nodes.emplace_back(root);
    growable.push_back(0);
}

void Builder::write_world(World& world, TerrainRenderer& terrain_renderer)
{
    world.clear();
    terrain_renderer.clear();

    for (auto& node: nodes) {
        Region region;
        region.centre = node.pos;
        region.visible = true;
        node.region = world.regions.size();
        world.regions.push_back(region);
    }
    for (const auto& node: nodes) {
        Region& region = world.regions[node.region];
        region.connection_start = world.connections.size();
        region.connections_count = node.edges.size();
        for (int edge: node.edges) {
            int next_region = nodes[edge].region;
            Connection connection;
            connection.region = next_region;
            world.connections.push_back(connection);
        }
    }

    std::vector<MeshVertex> vertices;
    std::vector<unsigned short> indices;
    for (int node_i = 0; node_i < nodes.size(); node_i++) {
        const Node& node = nodes[node_i];
        vertices.clear(); indices.clear();
        terrain_renderer.set_region(node.region);

        append_circle(vertices, indices,
            road_color,
            glm::vec3(0, 0, 1),
            road_width / 2,
            road_width * 0.05,
            node.pos);

        append_angle_set_mesh(
            vertices,
            indices,
            node.angle_set,
            node.pos + glm::vec3(0, 0, 0.2),
            glm::vec4(0, 0, 1, 1),
            road_width/2,
            road_width * 0.05);

        for (int edge_i: node.edges) {
            const Edge& edge = edges[edge_i];
            const Node& other = edge.node_1 != node_i ? nodes[edge.node_1] : nodes[edge.node_2];

            glm::vec3 end = (node.pos + other.pos) / 2.f;
            glm::vec3 disp = other.pos - node.pos;
            glm::vec3 u1 = glm::normalize(disp);
            append_plane(vertices, indices,
                road_color,
                glm::vec3(0, 0, 1),
                u1,
                glm::length(disp),
                road_width,
                node.pos + disp / 2.f);
        }
        terrain_renderer.load_mesh(vertices, indices);
    }

    terrain_renderer.update_buffers();
}

bool Builder::edge_valid(int node_from, const Capsule& capsule)
{
    for (const auto& edge: edges) {
        if (edge.node_1 == node_from || edge.node_2 == node_from) continue;
        auto result = capsule_collision(capsule, edge.capsule);
        if (result.collision) {
            return false;
        }
    }
    return true;
}

void Builder::append_angle_set_mesh(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const AngleSet& angle_set,
    const glm::vec3& pos,
    const glm::vec4& color,
    float radius,
    float resolution)
{
    size_t indices_offset = vertices.size();

    MeshVertex vertex;
    vertex.color = color;
    vertex.normal = glm::vec3(0, 0, 1);
    const glm::vec3 u1(1, 0, 0);
    const glm::vec3 u2(0, 1, 0);

    size_t N = ceil(2 * M_PI * radius / resolution);

    bool has_adjacent = false;
    std::vector<float> thetas(N);
    std::vector<bool> inside(N);
    for (size_t i = 0; i < N; i++) {
        thetas[i] = 2 * M_PI * (float)i / N;
        inside[i] = angle_set.contains(thetas[i]);
        if (i != 0) {
            has_adjacent |= inside[i] && inside[i-1];
        }
    }
    has_adjacent |= inside[0] && inside.back();
    if (!has_adjacent) return;

    vertex.pos = pos;
    vertices.push_back(vertex);

    for (size_t i = 0; i < N; i++) {
        if (inside[i]) {
            vertex.pos = pos + radius * u1 * std::cos(thetas[i]) + radius * u2 * std::sin(thetas[i]);
            vertices.push_back(vertex);
        }
        if (inside[i] && inside[(i + 1)%N]) {
            indices.push_back(indices_offset);
            indices.push_back(vertices.size() - 1);
            if (i == N - 1) {
                indices.push_back(indices_offset + 1);
            } else {
                indices.push_back(vertices.size());
            }
        }
    }
}
