#pragma once

#include "component/component.h"
#include <glm/glm.hpp>

struct Agent: public Component {
    glm::vec2 pos;
    float orient;

    int body_shape;
    int hand_shape_left;
    int hand_shape_right;
};
