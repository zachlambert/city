#pragma once

#include "world/world.h"
#include "system/terrain_renderer.h"
#include <glm/gtc/constants.hpp>
#include <memory>
#include <yaml-cpp/yaml.h>


inline float rand_float()
{
    return (float)rand() / (float)RAND_MAX;
}

class AreaBuilder {
public:
    virtual void generate(World& world, TerrainRenderer& terrain_renderer) = 0;
};

class Builder {
public:
    Builder(const YAML::Node& config):
        config(config)
    {}

    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args& ...args)
    {
        std::shared_ptr<T> root = std::make_shared<T>(config);
        this->root = root;
        return root;
    }

    void generate(World& world, TerrainRenderer& terrain_renderer)
    {
        root->generate(world, terrain_renderer);
        terrain_renderer.update_buffers();
    }

private:
    YAML::Node config; // Copy
    std::shared_ptr<AreaBuilder> root;
};
