

#include "world/builder.h"


Builder::Builder(YAML::Node config)
{
    angle_std = config["angle_std"].as<float>();
    angle_spacing = config["angle_spacing"].as<float>();
    min_length = config["min_length"].as<float>();
    max_length = config["max_length"].as<float>();
    road_width = config["road_width"].as<float>();
    max_iteration = config["max_iteration"].as<int>();
    auto road_color_arr = config["road_color"];
    for (size_t i = 0; i < road_color_arr.size(); i++) {
        road_color[i] = road_color_arr[i].as<float>();
    }
    min_distance = config["min_distance"].as<float>();
    snap_distance = config["snap_distance"].as<float>();
    node_snap_distance = config["node_snap_distance"].as<float>();
    edge_snap_angle = config["edge_snap_angle"].as<float>();

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

#if 0
        float angle = node.angle_set.get(rand_float(), angle_spacing);
#else
        float angle;
        {
            size_t i = rand() % node.angle_set.intervals.size();
            const AngleInterval& interval = node.angle_set.intervals[i];
            angle = clamp_angle(interval.min + std::clamp(sample_gaussian(interval.range/2, angle_std), 0.f, interval.range));
        }
#endif
        AngleInterval interval(angle, angle_spacing);
        AngleInterval opp_interval(clamp_angle(angle + M_PI), angle_spacing);

        float length = min_length + rand_float() * (max_length - min_length);
        glm::vec3 next_pos = node.pos + length * glm::vec3(std::cos(angle), std::sin(angle), 0);

        auto edge_query = query_edge(node_i, next_pos);
        if (edge_query.collision || edge_query.distance < snap_distance) {

            Edge& collided_edge = edges[edge_query.edge_i];
            Node& node_1 = nodes[collided_edge.node_1];
            Node& node_2 = nodes[collided_edge.node_2];

            auto snap_node = [this](Node& from, Node& to, int from_i, int to_i) {
                const glm::vec3 disp = to.pos - from.pos;
                float snap_angle = atan2(disp.y, disp.x);

                AngleInterval snap_interval(snap_angle, angle_spacing);
                AngleInterval opp_snap_interval(clamp_angle(snap_angle + M_PI), angle_spacing);

                if (to.angle_set.intersects(opp_snap_interval)) {
                    return false;
                }

                int snap_edge_i = edges.size();

                from.angle_set.remove(snap_interval);
                from.angle_set.cull(angle_spacing);
                from.edges.push_back(snap_edge_i);

                to.angle_set.remove(snap_interval);
                to.angle_set.cull(angle_spacing);
                to.edges.push_back(snap_edge_i);

                Edge snap_edge;
                snap_edge.node_1 = from_i;
                snap_edge.node_2 = to_i;
                snap_edge.capsule = Capsule(from.pos, to.pos, road_width / 2);
                edges.push_back(snap_edge);

                return true;
            };
            if (glm::length(edge_query.snap_pos - node_1.pos) < node_snap_distance) {
                return snap_node(node, node_1, node_i, collided_edge.node_1);
            }
            if (glm::length(edge_query.snap_pos - node_2.pos) < node_snap_distance) {
                return snap_node(node, node_2, node_i, collided_edge.node_2);
            }

            // Determine snap angle and road angle, to make sure these aren't
            // too close together

            const glm::vec3 disp = edge_query.snap_pos - node.pos;
            float snap_angle = atan2(disp.y, disp.x);

            const glm::vec3 road_disp = node_2.pos - node_1.pos;
            float road_angle = atan2(road_disp.y, road_disp.x);

            if (std::fabs(std::fabs(clamp_angle(road_angle - snap_angle)) - M_PI/2) > edge_snap_angle) {
                return false;
            }

            // Create the new node

            int next_i = nodes.size();
            Node next(edge_query.snap_pos, node.iteration + 1);

            // Create the snap edge

            int snap_edge_i = edges.size();
            Edge snap_edge;
            snap_edge.node_1 = node_i;
            snap_edge.node_2 = next_i;
            snap_edge.capsule = Capsule(node.pos, edge_query.snap_pos, road_width / 2);

            // Split the collided edge into two edges
            // [node_1] -> collided -> [node_2]
            // =>
            // [node_1] -> collided -> [next] -> other -> [node_2]

            int other_edge_i = edges.size() + 1;
            Edge other_edge;
            other_edge.node_1 = next_i;
            other_edge.node_2 = collided_edge.node_2;
            other_edge.capsule = Capsule(edge_query.snap_pos, node_2.pos, road_width / 2);

            collided_edge.node_2 = next_i;
            collided_edge.capsule.end = edge_query.snap_pos;

            // Define road and snap intervals, and apply

            AngleInterval snap_interval(snap_angle, angle_spacing);
            AngleInterval opp_snap_interval(clamp_angle(snap_angle + M_PI), angle_spacing);
            AngleInterval road_interval(road_angle, angle_spacing);
            AngleInterval opp_road_interval(clamp_angle(road_angle + M_PI), angle_spacing);

            // Update [next] angle_set and edges

            next.angle_set.remove(opp_snap_interval);
            next.angle_set.remove(road_interval);
            next.angle_set.remove(opp_road_interval);
            next.angle_set.cull(angle_spacing);
            next.edges.push_back(snap_edge_i);
            next.edges.push_back(edge_query.edge_i);
            next.edges.push_back(other_edge_i);

            // Update [node] angle_set and edges

            node.edges.push_back(snap_edge_i);
            node.angle_set.remove(snap_interval);
            node.angle_set.cull(angle_spacing);

            // Update [node_2] edges
            // (node_1 and node_2 have angle sets unchanged)
            // (node_1 has edges unchanged)

            node_2.edges.erase(std::find(node_2.edges.begin(), node_2.edges.end(), edge_query.edge_i));
            node_2.edges.push_back(other_edge_i);

            // Push the new node, and two new edges

            edges.push_back(snap_edge);
            edges.push_back(other_edge);
            nodes.push_back(next);

            if (next.iteration < max_iteration) {
                growable.push_back(next_i);
            }

            return true;
        }

        node.angle_set.remove(interval);
        node.angle_set.cull(angle_spacing);

        if (edge_query.distance < min_distance) {
            continue;
        }

        int next_i = nodes.size();

        Edge edge;
        edge.node_1 = node_i;
        edge.node_2 = next_i;
        edge.capsule = Capsule(node.pos, next_pos, road_width / 2);
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

Builder::EdgeQuery Builder::query_edge(int node_from, const glm::vec3& to)
{
    Capsule capsule(nodes[node_from].pos, to, road_width / 2);

    EdgeQuery query;
    query.collision = false;
    query.distance = INFINITY;

    for (int edge_i = 0; edge_i < edges.size(); edge_i++) {
        const Edge& edge = edges[edge_i];
        if (edge.node_1 == node_from || edge.node_2 == node_from) continue;
        auto result = capsule_collision(capsule, edge.capsule);

        if (result.collision) {
            float dist_to_start = glm::length(result.closest_a - nodes[node_from].pos);
            if (!query.collision || dist_to_start < query.distance) {
                query.edge_i = edge_i;
                query.collision = true;
                query.distance = dist_to_start;
                query.snap_pos = result.closest_b;
            }

        } else if (!query.collision && result.distance < query.distance) {
            query.edge_i = edge_i;
            query.collision = false;
            query.distance = result.distance;
            query.snap_pos = result.closest_b;
        }
    }
    // Use query.distance to find the closest intersection, but change
    // it to zero in the return value, since it should be the min distance
    // to an obstacle
    if (query.collision) {
        query.distance = 0;
    }

    return query;
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
