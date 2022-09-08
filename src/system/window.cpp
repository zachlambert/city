
#include "system/window.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <iostream>


WindowState* g_window_state = nullptr;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (!g_window_state) return;
    g_window_state->key_states[key] = action;
}

void char_callback(GLFWwindow* window, unsigned int codepoint)
{
    if (!g_window_state) return;
    g_window_state->char_input = codepoint;
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!g_window_state) return;
    g_window_state->mouse_pos_screen = { xpos, ypos };
    g_window_state->mouse_active = true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (!g_window_state) return;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        g_window_state->mouse_right_action = action;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        g_window_state->mouse_left_action = action;
    }
}

Window::Window(
    WindowState& window_state,
    const GameState& game_state,
    const Args& args
):
    window_state(window_state),
    game_state(game_state),
    bg(args.bg)
{
    glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (args.fullscreen) {
        window = glfwCreateWindow(args.width, args.height, args.name.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window = glfwCreateWindow(args.width, args.height, args.name.c_str(), NULL, NULL);
    }

    if (!window) {
        std::cerr << "Failed to open GLFW window.\n";
        return;
    }
    glfwMakeContextCurrent(window); // Initialise GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialise GLEW.\n";;
        return;
    }

    // TODO: Provide configuration for this
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Disable vsync
    // glfwSwapInterval(0);

    window_state.screen_width = args.width;
    window_state.screen_height = args.height;
    window_state.aspect_ratio = (float)args.width / args.height;

    window_state.mouse_active = false;
    window_state.mouse_pos_screen = { 0, 0 };
    window_state.mouse_pos_world = { 0, 0 };
    window_state.char_input = 0;
    for (const auto& key: args.keys) {
        window_state.key_states.emplace(key, GLFW_KEY_UP);
    }

    window_state.view_active = true;
    window_state.running = true;

    g_window_state = &window_state;
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
}

Window::~Window()
{
    g_window_state = nullptr;
    glfwTerminate();
}

void Window::tick()
{
    window_state.running = !glfwWindowShouldClose(window);
    glfwSwapBuffers(window);

    window_state.char_input = 0;
    glfwPollEvents();

    const float scale = game_state.camera.zoom / game_state.camera.nominal_view_size;
    window_state.view_matrix =
        glm::scale(glm::vec3(scale, scale * window_state.aspect_ratio, 1))
        * glm::translate(glm::vec3(-game_state.camera.pose.pos().x, -game_state.camera.pose.pos().y, 0))
        * glm::rotate(game_state.camera.pose.theta(), glm::vec3(0, 0, 1));

    {
        // Convert mouse pos to screen coords, ie: -1 -> 1
        glm::vec4 screen_coords(0.0f, 0.0f, 0.0f, 1.0f);
        screen_coords.x = 2 * (
            window_state.mouse_pos_screen.x
                - (float)window_state.screen_width / 2
            ) / window_state.screen_width;
        screen_coords.y = - 2 * (
            window_state.mouse_pos_screen.y
                - (float)window_state.screen_height / 2
            ) / window_state.screen_height;

        // Project onto world
        glm::mat4 view_inverse =
            glm::translate(glm::vec3(game_state.camera.pose.pos().x, game_state.camera.pose.pos().y, 0))
            * glm::scale(glm::vec3(1.0f / scale, 1.0f / (scale * window_state.aspect_ratio), 1));
        window_state.mouse_pos_world = view_inverse * screen_coords;
    }


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glClearColor(bg.x, bg.y, bg.z, bg.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
