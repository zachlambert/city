#pragma once

#include "component/component.h"
#include <glm/glm.hpp>


struct Circle {
    struct Args {
        glm::vec4 color;
        float radius;
        float edge_width;
    };

    glm::vec4 color; 
    glm::vec2 pos;
    float radius;
    float edge_width;
    bool valid;
};
typedef ComponentList<Circle> CircleList;

int create_circle(
    CircleList& circles,
    const Circle::Args& args
);

void destroy_circle(
    CircleList& circles,
    int index
);
