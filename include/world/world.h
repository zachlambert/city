#pragma once

#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <unordered_map>


struct Region {
    int num_children;
    // Connections are guaranteed to be right after the region element
    bool visible;
};

struct RegionConnection {
    int child; // -> Region
};

struct WorldGraph {
    typedef std::variant<
        Region,
        RegionConnection
    > Element;
    std::vector<Element> elements;
};

enum class RegionType {
    GROUP, // No geometry,
    OUTSIDE_ROAD,
    OUTSIDE_JUNCTION,
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
    WorldGraph graph;
    std::unordered_map<int, RegionMetadata> region_metadatas;
};
