#pragma once

#include "world/world.h"
#include "system/terrain_renderer.h"

class Builder {
public:
    Builder(
        World& world,
        TerrainRenderer& terrain_renderer);

    void build();

private:
    World& world;
    TerrainRenderer& terrain_renderer;
};
