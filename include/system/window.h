#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "state/window_state.h"
#include "state/game_state.h"


class Window {
public:
    struct Args {
        std::string name;
        int width;
        int height;
        bool fullscreen;
        glm::vec4 bg;
        std::vector<int> keys;
    };
    Window(
        WindowState& window_state,
        const GameState& game_state,
        const Args& args);
    ~Window();
    void tick();

private:
    WindowState& window_state;
    const GameState& game_state;

    GLFWwindow *window;
    glm::vec4 bg;
    glm::mat4 p_rot; // required rotation before projection
};
