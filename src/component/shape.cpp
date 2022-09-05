
#include "state.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


int State::create_shape()
{
    int index = shapes.create();

    Shape& shape = shapes[index];
    shape.color = { 1, 0, 0, 1 };
    Shape::Circle circle;
    circle.radius = 1.0;
    circle.edge_width = 0;
    shape.shape = circle;

    return index;
}

void State::remove_shape(int index)
{
    const Shape& shape = shapes[index];

    shapes.remove(index);
}

void State::update_shapes()
{
    shape_renderer.circle_instances.clear();

    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape& shape = shapes[i];
        if (!shape.valid) continue;

        if (auto circle = std::get_if<Shape::Circle>(&shape.shape)) {
            ShapeRenderer::CircleInstance instance;
            instance.pos = { shape.pos.x, shape.pos.y, 1 };
            instance.color = shape.color;
            instance.radius = circle->radius;
            instance.edge_width = circle->edge_width;
            shape_renderer.circle_instances.push_back(instance);
        }
    }
}

