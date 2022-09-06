
#include "state.h"
#include <iostream>

void State::create_window()
{
    assert(!window.valid);

    window.name = "City";
    window.width = 1200;
    window.height = 800;
    window.fullscreen = false;
    window.bg = { 0.6, 0.6, 0.6, 1 };
    window.shader_path = "shaders/";

    window.aspect_ratio = (float)window.width / window.height;

    window.have_world_view = false;
    window.running = false;

    glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (window.fullscreen) {
        window.window = glfwCreateWindow(window.width, window.height, window.name.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window.window = glfwCreateWindow(window.width, window.height, window.name.c_str(), NULL, NULL);
    }

    if (!window.window) {
        std::cerr << "Failed to open GLFW window.\n";
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window.window); // Initialise GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialise GLEW.\n";;
        return;
    }

    // TODO: Provide configuration for this
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Disable vsync
    // glfwSwapInterval(0);

    window.running = true;
    window.valid = true;
}

void State::remove_window()
{
    assert(window.valid);
    glfwTerminate();
    window.valid = false;
}

void State::update_window()
{
    window.running = !glfwWindowShouldClose(window.window);
    glfwSwapBuffers(window.window);
    glfwPollEvents();

    // TODO: Read input

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glClearColor(window.bg.x, window.bg.y, window.bg.z, window.bg.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
