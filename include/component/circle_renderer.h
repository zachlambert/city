#pragma once

#include "component/component.h"
#include <vector>
#include <array>
#include <glm/glm.hpp>


struct CircleRenderer: public Component {
    std::array<glm::vec2, 6> quad_vertices;

    struct Instance {
        glm::vec3 pos; // z = depth
        glm::vec4 color;
        float radius;
        float edge_width;
    };

    unsigned int VAO;
    unsigned int quad_VBO;
    unsigned int instance_VBO;

    unsigned int program_id;
    unsigned int v_loc;

    std::vector<Instance> instances;
};
