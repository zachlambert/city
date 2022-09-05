#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "component/component.h"


struct Window: public Component {
    // Settings
    std::string name;
    unsigned int width;
    unsigned int height;
    bool fullscreen;
    glm::vec4 bg;
    std::string shader_path;

    // Window objects
    GLFWwindow *window;
    float aspect_ratio;

    // State
    bool running;
    glm::mat4 view_matrix;
    glm::vec2 mouse_pos;
    bool have_world_view;
};
