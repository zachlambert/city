
#include "world/builder.h"

Builder::Builder(
    World& world,
    TerrainRenderer& terrain_renderer
):
    world(world),
    terrain_renderer(terrain_renderer)
{}

void Builder::build()
{
    Region root;
    root.visible = true;
    root.num_children = 0;
    world.graph.elements.push_back(root);

    RegionMetadata root_metadata;
    root_metadata.centre = glm::vec3(0, 0, 0);
    root_metadata.type = RegionType::OUTSIDE_JUNCTION;
    world.region_metadatas.emplace(0, root_metadata);

    std::vector<MeshVertex> vertices;
    std::vector<unsigned short> indices;

    terrain_renderer.set_region(0);
    {
        generate_plane(vertices, indices,
            glm::vec4(0.8, 0.8, 0.8, 1),
            glm::vec3(0, 0, 1),
            glm::vec3(1, 0, 0),
            10,
            10);
        terrain_renderer.load_mesh(vertices, indices);
    }
    terrain_renderer.update_buffers();
}
