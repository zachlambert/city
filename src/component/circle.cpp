
#include "state.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


int State::create_circle(Circle::Args args)
{
    int index = circles.create();

    Circle& circle = circles[index];
    circle.color = args.color;
    circle.radius = args.radius;
    circle.edge_width = args.edge_width;

    return index;
}

void State::remove_circle(int index)
{
    const Circle& circle = circles[index];

    circles.remove(index);
}

void State::update_circles()
{
    circle_renderer.instances.clear();

    for (size_t i = 0; i < circles.size(); i++) {
        const Circle& circle = circles[i];
        if (!circle.valid) continue;

        CircleRenderer::Instance instance;
        instance.pos = { circle.pos.x, circle.pos.y, 1 };
        instance.color = circle.color;
        instance.radius = circle.radius;
        instance.edge_width = circle.edge_width;
        circle_renderer.instances.push_back(instance);
    }
}

