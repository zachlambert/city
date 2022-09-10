#pragma once

#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <unordered_map>


struct Region {
    int connection_start;
    int num_neighbours;
    int num_children;
    // Connections are guaranteed to be right after the region element
    // [ <region> <neighbour connections> <children connections> ]
    bool visible;
    // If physical false, this region doesn't have any associated terrain,
    // but serves as a grouping of smaller regions that are all inside it
    bool physical;
};

enum class RegionType {
    OUTSIDE,
    OUTSIDE_ROAD,
    OUTSIDE_JUNCTION,
    INSIDE,
    INSIDE_ROOM
};

struct RegionMetadata {
    glm::vec3 centre;
    struct Interface {
        glm::vec3 a;
        glm::vec3 b;
    };
    std::vector<Interface> interfaces;
    RegionType type;
};

struct World {
    std::vector<Region> regions;
    std::vector<int> connections;
    std::vector<RegionMetadata> region_metadatas;
};
