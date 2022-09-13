#pragma once

#include <yaml-cpp/yaml.h>
#include "world/builder.h"
#include "system/terrain_renderer.h"
#include "state/input.h"


class BuildManager {
public:
    BuildManager(
        const Input& input,
        World& world,
        TerrainRenderer& terrain_renderer,
        YAML::Node config
    ):
        input(input),
        world(world),
        terrain_renderer(terrain_renderer),
        builder(config["builder"])
    {

    }

    void tick()
    {
        if (input.key_states.at(GLFW_KEY_3).pressed()) {
            while (!builder.add_road());
            builder.write_world(world, terrain_renderer);
        }
        if (input.key_states.at(GLFW_KEY_4).pressed()) {
            builder.clear();
            builder.write_world(world, terrain_renderer);
        }
    }

private:
    const Input& input;
    World& world;
    TerrainRenderer& terrain_renderer;
    Builder builder;
};
