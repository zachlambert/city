#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "state/viewport.h"
#include "state/input.h"
#include "state/camera.h"


class WindowHandler {
public:
    struct Args {
        std::string name;
        int width;
        int height;
        bool fullscreen;
        glm::vec4 bg;
        std::vector<int> keys;
    };
    WindowHandler(
        Viewport& viewport,
        Input& input,
        const Camera& camera,
        const Args& args);
    ~WindowHandler();
    void tick();

private:
    Viewport& viewport;
    Input& input;
    const Camera& camera;

    GLFWwindow *window;
    glm::vec4 bg;
};
