#pragma once

#include "component/component.h"
#include <glm/glm.hpp>
#include <variant>

struct Circle: public Component {
    struct Args {
        glm::vec4 color;
        float radius;
        float edge_width;
    };
    glm::vec2 pos;
    glm::vec4 color; 
    float radius;
    float edge_width;
};
