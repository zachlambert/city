#pragma once

#include "component/component.h"
#include <glm/glm.hpp>

struct Camera: public Component {
    struct Args {
        int agent = -1;
    };

    int agent;
    glm::vec2 pos;
    float zoom;
    int priority;
};
