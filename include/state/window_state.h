#pragma once

#include <glm/glm.hpp>
#include <unordered_map>


struct WindowState {
    int screen_width;
    int screen_height;
    float aspect_ratio;

    bool mouse_active;
    glm::vec2 mouse_pos_screen;
    glm::vec2 mouse_pos_world;
    unsigned int char_input; // 0 if no char received last poll
    std::unordered_map<int, int> key_states;
    int mouse_left_action;
    int mouse_right_action;

    bool view_active;
    glm::mat4 view_matrix;

    bool running;
};
