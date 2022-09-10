#pragma once

#include <glm/glm.hpp>


struct Viewport {
    int screen_width;
    int screen_height;
    float aspect_ratio;

    bool view_active;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;

    bool open;
};
