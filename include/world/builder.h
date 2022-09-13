#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <stack>
#include "world/world.h"
#include "system/terrain_renderer.h"
#include "maths/geometry.h"


inline float rand_float()
{
    return (float)rand() / (float)RAND_MAX;
}

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

class Builder {
public:
    Builder()
    {}

    void build(World& world, TerrainRenderer& terrain_renderer)
    {
        float angle_spacing = M_PI / 2;
        float min_length = 30;
        float max_length = 100;
        float road_width = 5;
        int max_iteration = 20;
        glm::vec4 road_color = glm::vec4(0.4, 0.4, 0.4, 1);

        nodes.emplace_back(glm::vec3(0, 0, 0), 0);
        printf("Initial interval\n");
        nodes[0].angle_set.print();
        growable.push(0);
        while (!growable.empty()) {
            int node_i = growable.top();
            auto& node = nodes[node_i];
            printf("Extending from %i\n", node_i);
            node.angle_set.print();
            printf("Edges: ");
            for (int conn: node.edges) printf("%i, ", conn);
            printf("\n");

            if (node.angle_set.empty()) {
                printf("Finished %i\n", node_i);
                growable.pop();
                continue;
            }

            float length = min_length + rand_float() * (max_length - min_length);
            float angle = node.angle_set.get(rand_float(), angle_spacing/2);
            AngleInterval interval(angle, angle_spacing);
            AngleInterval opp_interval(clamp_angle(angle + M_PI), angle_spacing);

            glm::vec3 next_pos = node.pos + length * glm::vec3(std::cos(angle), std::sin(angle), 0);

            auto snap = find_snap(node_i, node.pos, next_pos);
            if (snap.node != -1 && !snap.on_edge) {
                printf("Node snap to %i\n", snap.node);
                Node& next = nodes[snap.node];
                float angle = std::atan2(next.pos.y - node.pos.y, next.pos.x - node.pos.x);

                node.angle_set.remove(interval);
                node.angle_set.cull(angle_spacing);

                continue; // TEMP

                // If the next node can't make this connection, continue
                if (next.angle_set.intersects(opp_interval)) {
                    continue;
                }

                node.edges.push_back(snap.node);

                next.angle_set.remove(opp_interval);
                next.angle_set.cull(angle_spacing);
                next.edges.push_back(node_i);

                continue;
            }
            if (snap.node != -1 && snap.on_edge) {
                printf("Edge snap to %i<->%i\n", snap.node, nodes[snap.node].edges[snap.edge]);
                int next_i = nodes.size();
                Node next(snap.edge_pos, node.iteration + 1);

                node.angle_set.remove(interval);
                node.angle_set.cull(angle_spacing);

                continue; // TEMP

                Node& snap_a = nodes[snap.node];
                Node& snap_b = nodes[snap_a.edges[snap.edge]];

                float road_angle = std::atan2(snap_b.pos.y - snap_a.pos.y, snap_b.pos.x - snap_a.pos.x);
                float angle_dif = std::fabs(std::fabs(clamp_angle(road_angle - angle)) - M_PI/2);
                if (angle_dif > M_PI/4) {
                    printf("Failed\n");
                    continue;
                }
                node.edges.push_back(next_i);

                for (auto& edge: snap_b.edges) {
                    if (edge == snap.node) {
                        edge = next_i;
                        break;
                    }
                }

                AngleInterval road_interval(road_angle, angle_spacing);
                AngleInterval opp_road_interval(clamp_angle(road_angle + M_PI), angle_spacing);
                next.angle_set.remove(road_interval);
                next.angle_set.remove(opp_road_interval);
                next.angle_set.remove(opp_interval);
                next.angle_set.cull(angle_spacing);
                next.edges.push_back(snap.node);
                next.edges.push_back(snap_a.edges[snap.edge]);
                next.edges.push_back(node_i);

                snap_a.edges[snap.edge] = next_i;
                if (next.iteration < max_iteration) {
                    // growable.push(next_i);
                }
                nodes.push_back(next);

                continue;
            }

            printf("Free node\n");

            // Update node before adding data to nodes, which can invalidate
            // the reference
            int next_i = nodes.size();
            node.edges.push_back(next_i);
            node.angle_set.remove(interval);
            node.angle_set.cull(angle_spacing);
            int next_iteration = node.iteration + 1;

            nodes.emplace_back(next_pos, next_iteration);
            Node& next = nodes.back();
            next.edges.push_back(node_i);
            next.angle_set.remove(opp_interval);
            next.angle_set.cull(angle_spacing);

            if (next.iteration < max_iteration) {
                growable.push(next_i);
            } else {
                printf("Terminal node\n");
            }
        }

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

                auto iter = world_conn_datas.find({node.region, next_region});
                if (iter == world_conn_datas.end()) {

                    connection.data = world.connection_datas.size();
                    ConnectionData data;

                    glm::vec3 centre = (node.pos + nodes[edge].pos) / 2.f;
                    glm::vec3 disp = node.pos - nodes[edge].pos;
                    glm::vec3 dir = glm::normalize(glm::cross(glm::vec3(0, 0, 1), disp));
                    data.start = centre - dir * road_width / 2.f;
                    data.start = centre + dir * road_width / 2.f;
                    world.connection_datas.push_back(data);

                    world_conn_datas.emplace(ConnDataQuery(node.region, next_region), connection.data);

                } else {
                    connection.data = iter->second;
                }

                world.connections.push_back(connection);
            }
        }

        std::vector<MeshVertex> vertices;
        std::vector<unsigned short> indices;
        for (const auto& node: nodes) {
            vertices.clear(); indices.clear();
            terrain_renderer.set_region(node.region);

            append_circle(vertices, indices,
                // road_color,
                glm::vec4(1, 0, 0, 1),
                glm::vec3(0, 0, 1),
                road_width / 2,
                road_width * 0.05,
                node.pos + glm::vec3(0, 0, 0.5));

            for (int edge: node.edges) {
                const Node& other = nodes[edge];
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

private:
    struct Snap {
        int node;
        bool on_edge;
        int edge;
        glm::vec3 edge_pos;
    };
    Snap find_snap(int from_node, const glm::vec3& from, const glm::vec3& pos)
    {
        float snap_dist = 20;
        float edge_snap_spacing = 20;
        bool colliding = false;

        // Not efficient, but just getting something working for now
        Snap min_snap;
        min_snap.node = -1;
        float min_dist = INFINITY;
        for (int i = 0; i < nodes.size(); i++) {
            if (i == from_node) continue;
            bool already_connected = false;
            for (int edge: nodes[i].edges) {
                if (edge == from_node) {
                    already_connected = true;
                }
            }
            if (already_connected) continue;

            float dist = glm::length(nodes[i].pos - pos);
            if (!colliding && dist < min_dist) {
                min_dist = dist;
                min_snap.node = i;
                min_snap.on_edge = false;
            }
            for (int j = 0; j < nodes[i].edges.size(); j++) {
                if (nodes[i].edges[j] == from_node) continue;
                glm::vec3 a = nodes[i].pos;
                a.z = 0;
                glm::vec3 b = nodes[nodes[i].edges[j]].pos;
                b.z = 0;
                glm::vec3 u1 = normalize(b - a);
                glm::vec3 u2 = glm::cross(glm::vec3(0, 0, 1), u1);

                float x1_to = glm::dot(pos - a, u1);
                float x1_line = glm::length(b - a);

                // 1. Do the lines intersect?
                // Check for intersections on the 2D plane
                while (true) {
                    float x2_from = glm::dot(from - a, u2);
                    float x2_to = glm::dot(pos - a, u2);
                    // No intersection if these have the same sign
                    if (x2_from * x2_to > 0) break;
                    float x1_from = glm::dot(from - a, u1);
                    float x1_end =
                        x1_from
                        + (std::fabs(x2_from) / (std::fabs(x2_from) + std::fabs(x2_to)))
                            * (x1_to - x1_from);
                    if (x1_end > x1_line) break;

                    colliding = true;

                    glm::vec3 edge_pos = nodes[i].pos
                        + (nodes[nodes[i].edges[j]].pos - nodes[i].pos) * (x1_end / x1_line);
                    float dist = glm::length(edge_pos - from);
                    if (dist > min_dist) break;

                    min_snap.node = i;
                    min_snap.on_edge = true;
                    min_snap.edge = j;
                    min_snap.edge_pos = edge_pos;
                    colliding = true;
                    min_dist = glm::length(min_snap.edge_pos - from);
                    break;
                }

                if (colliding) continue;

                // 2. If not, is the new point sufficiently close to snap?

                // Check it isn't too close to the nodes
                if (x1_to < edge_snap_spacing || x1_to >= x1_line - edge_snap_spacing) {
                    continue;
                }

                float edge_dist = glm::length(a + u1 * x1_to - pos);
                if (edge_dist < min_dist) {
                    min_snap.node = i;
                    min_snap.on_edge = true;
                    min_snap.edge = j;
                    min_snap.edge_pos = a + u1 * x1_to;
                    min_dist = edge_dist;
                }
            }
        }
        if (min_dist > snap_dist) {
            min_snap.node = -1;
        }
        return min_snap;
    }

    struct ConnDataQuery {
        int lower;
        int higher;
        ConnDataQuery() {}
        ConnDataQuery(int a, int b)
        {
            lower = std::min(a, b);
            higher = std::max(a, b);
        }
        bool operator==(const ConnDataQuery &query)const {
            return lower == query.lower && higher == query.higher;
        }
    };
    struct ConnDataQueryHash {
        std::size_t operator()(const ConnDataQuery& query) const
        {
            std::size_t h1 = std::hash<int>()(query.lower);
            std::size_t h2 = std::hash<int>()(query.higher);
            return h1 ^ h2;
        }
    };
    std::vector<Node> nodes;
    std::stack<int> growable;
    std::unordered_map<ConnDataQuery, int, ConnDataQueryHash> world_conn_datas;
};
