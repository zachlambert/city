#pragma once

#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <unordered_map>


struct Region {
    int connection_start;
    int connections_count;
    glm::vec3 centre;
    bool visible;
    Region():
        connection_start(-1),
        connections_count(0),
        centre(0, 0, 0),
        visible(true)
    {}
};

struct Connection {
    int region;
    int data;
};

struct ConnectionData {
    glm::vec2 start;
    glm::vec2 end;
};

struct World {
    std::vector<Region> regions;
    std::vector<Connection> connections;
    std::vector<ConnectionData> connection_datas;
};
