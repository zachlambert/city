
#include "world/builder.h"
#include <stack>


Builder::Builder():
    next_node(0)
{}

void Builder::finish(World& world, TerrainRenderer& terrain_renderer)
{
    std::vector<MeshVertex> vertices;
    std::vector<unsigned short> indices;

    struct StackState {
        int node;
        int region;
        int num_neighbours;
        int child;
        StackState(int node, int region, int num_neighbours):
            node(node), region(region), num_neighbours(num_neighbours), child(-1)
        {}
    };
    std::stack<StackState> stack;
    stack.emplace(0, 0, 0);
    world.regions.emplace_back();

    while (!stack.empty()) {
        StackState& state = stack.top();
        nodes.at(state.node).region = state.region;
        const auto& node = nodes.at(state.node);

        if (state.child == -1){
            bool physical = node.children.empty();

            Region& region = world.regions[state.region];
            region.num_neighbours = state.num_neighbours;
            region.num_children = node.children.size();
            region.connection_start = world.connections.size();
            region.visible = true;
            region.physical = physical;

            // Reserve space for connections
            int num_connections = state.num_neighbours + node.children.size();
            for (int i = 0; i < num_connections; i++) {
                world.connections.emplace_back();
            }

            if (physical) {
                terrain_renderer.set_region(state.region);
                vertices.clear();
                indices.clear();
                // TODO: Change the mesh generation functions to not clear
                node.region_builder->generate_mesh(vertices, indices);
                terrain_renderer.load_mesh(vertices, indices);
            }

            state.child = 0;

        } else if (state.child < node.children.size()) {
            // Guaranteed that children.size() is at least 1
            int next_node = node.children[state.child];
            state.child++;
            stack.emplace(StackState(next_node, world.regions.size(), node.children.size() - 1));
            world.regions.emplace_back();

        } else {
            int conn_start = world.regions[state.region].connection_start;
            for (int i = 0; i < node.children.size(); i++) {
                int i_region = nodes.at(node.children[i]).region;
                world.connections[conn_start + state.num_neighbours + i] = i_region;

                int neighbour = 0;
                for (int j = 0; j < node.children.size(); j++) {
                    if (i == j) continue;
                    int j_region = nodes.at(node.children[j]).region;
                    int child_conn_start = world.regions[j_region].connection_start;
                    world.connections[child_conn_start + neighbour] = i_region;
                    neighbour++;
                }
            }
            stack.pop();
        }
    }

    terrain_renderer.update_buffers();
}

CityBuilder::CityBuilder(Builder& builder, int node):
    RegionBuilder(builder, node),
    centre(0, 0),
    size(0, 0),
    road_width(0),
    outer_padding(0)
{

}

void CityBuilder::create_children()
{
    roads.push_back(builder.create_region<RoadBuilder>(node));
    roads.back()->lower = centre + glm::vec2(
        -size.x/2 + outer_padding,
        -size.y/2 + outer_padding);
    roads.back()->upper = centre + glm::vec2(
        size.x/2 - outer_padding,
        -size.y/2 + outer_padding + road_width);

    roads.push_back(builder.create_region<RoadBuilder>(node));
    roads.back()->lower = centre + glm::vec2(
        -size.x/2 + outer_padding,
        size.y/2 - outer_padding - road_width);
    roads.back()->upper = centre + glm::vec2(
        size.x/2 - outer_padding,
        size.y/2 - outer_padding);

    roads.push_back(builder.create_region<RoadBuilder>(node));
    roads.back()->lower = centre + glm::vec2(
        -size.x/2 + outer_padding,
        -size.y/2 + outer_padding + road_width);
    roads.back()->upper = centre + glm::vec2(
        -size.x/2 + outer_padding + road_width,
        size.y/2 - outer_padding - road_width);

    roads.push_back(builder.create_region<RoadBuilder>(node));
    roads.back()->lower = centre + glm::vec2(
        size.x/2 - outer_padding - road_width,
        -size.y/2 + outer_padding + road_width);
    roads.back()->upper = centre + glm::vec2(
        size.x/2 - outer_padding,
        size.y/2 - outer_padding - road_width);
}

void CityBuilder::generate_mesh(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices)
{
    // Nothing
}

RoadBuilder::RoadBuilder(Builder& builder, int node):
    RegionBuilder(builder, node),
    lower(0, 0),
    upper(0, 0),
    color(0, 0, 0, 1)
{

}

void RoadBuilder::generate_mesh(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices)
{
    vertices.clear(); indices.clear();

    append_plane(vertices, indices,
        color,
        glm::vec3(0, 0, 1),
        glm::vec3(1, 0, 0),
        upper.x - lower.x,
        upper.y - lower.y);
    glm::vec3 pos;
    pos.x = 0.5f * (lower.x + upper.x);
    pos.y = 0.5f * (lower.y + upper.y);
    pos.z = 0;
    translate_vertices(vertices, pos);
}
