
#include "state.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


int State::create_shape()
{
    int index = shapes.create();

    Shape& shape = shapes[index];

    return index;
}

void State::remove_shape(int index)
{
    const Shape& shape = shapes[index];

    shapes.remove(index);
}

void State::update_shapes()
{
    for (size_t i = 0; i < shapes.size(); i++) {
        const Shape& shape = shapes[i];
        if (!shape.valid) continue;

        if (auto circle = std::get_if<Shape::Circle>(&shape.shape)) {
            std::cout << "Circle, pos = " << glm::to_string(shape.pos) << "\n";
        }
        else if (auto rect = std::get_if<Shape::Rect>(&shape.shape)) {
            std::cout << "Rect, pos = " << glm::to_string(shape.pos) << "\n";
        }
        else if (auto line = std::get_if<Shape::Line>(&shape.shape)) {
            std::cout << "Line, pos = " << glm::to_string(shape.pos) << "\n";
        }
    }
}

