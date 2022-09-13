#pragma once

#include <unordered_map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


struct KeyState {
    bool down;
    mutable bool just_changed;
    bool pressed()const
    {
        if (down && just_changed) {
            just_changed = false;
            return true;
        }
        return false;
    };
    bool released()const
    {
        if (!down && just_changed) {
            just_changed = false;
            return true;
        }
        return false;
    };

    KeyState():
        down(false),
        just_changed(false)
    {}
};

struct Input {
    bool mouse_active;
    glm::vec2 mouse_pos_screen;
    glm::vec2 mouse_pos_world;
    glm::vec2 mouse_delta;

    KeyState mouse_left_state;
    KeyState mouse_right_state;

    std::unordered_map<int, KeyState> key_states;

    unsigned int char_input; // 0 if no char received last poll
};

struct WindowState {
    int screen_width;
    int screen_height;
    float aspect_ratio;

    bool mouse_active;
    glm::vec2 mouse_pos_screen;
    glm::vec2 mouse_pos_world;
    glm::vec2 mouse_delta;

    bool view_active;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;

    bool running;
};
