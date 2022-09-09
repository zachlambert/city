#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include "state/input_state.h"


struct WindowState {
    int screen_width;
    int screen_height;
    float aspect_ratio;

    bool mouse_active;
    glm::vec2 mouse_pos_screen;
    glm::vec2 mouse_pos_world;
    glm::vec2 mouse_delta;

    unsigned int char_input; // 0 if no char received last poll
    std::unordered_map<int, KeyState> key_states;
    int mouse_left_action;
    int mouse_right_action;

    bool view_active;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;

    bool running;
};
