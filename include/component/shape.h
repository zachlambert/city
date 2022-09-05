#pragma once

#include "component/component.h"
#include <glm/glm.hpp>
#include <variant>

struct Shape: public Component {
    struct Circle {
        float radius;
        float edge_width;
    };
    struct Rect {
        float width;
        float height;
        float orient;
    };
    struct Line {
        glm::vec2 displacement;
        float width;
    };

    glm::vec2 pos;
    std::variant<Circle, Rect, Line> shape;
    glm::vec4 color; 
};
