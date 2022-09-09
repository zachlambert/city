
#include "system/window.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <iostream>


WindowState* g_window_state = nullptr;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (!g_window_state) return;

    if (action == GLFW_PRESS) {
        g_window_state->key_states[key].just_changed = true;
        g_window_state->key_states[key].down = true;
    } else if (action == GLFW_RELEASE) {
        g_window_state->key_states[key].just_changed = true;
        g_window_state->key_states[key].down = false;
    }
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
    KeyState* key_state = nullptr;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        key_state = &g_window_state->mouse_left_state;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        key_state = &g_window_state->mouse_right_state;
    }
    if (!key_state) return;

    if (action == GLFW_PRESS) {
        key_state->just_changed = true;
        key_state->down = true;
    } else if (action == GLFW_RELEASE) {
        key_state->just_changed = true;
        key_state->down = false;
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
        window_state.key_states.emplace(key, KeyState());
    }

    window_state.view_active = true;
    window_state.running = true;

    g_window_state = &window_state;
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
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

    glfwSetInputMode(window, GLFW_CURSOR,
        game_state.mouse_mode.cursor_hidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, 
        game_state.mouse_mode.raw_mouse_motion ? GLFW_TRUE : GLFW_FALSE);

    window_state.char_input = 0;
    glfwPollEvents();

    window_state.view_matrix = game_state.camera.pose.inverse().to_mat4();
    const float scale = game_state.camera.zoom / game_state.camera.nominal_view_size;
    window_state.projection_matrix = glm::perspective(
        (float)M_PI/4, window_state.aspect_ratio, 0.01f, 100.0f
    ) * glm::mat4(coord_system_fix());

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
            game_state.camera.pose.to_mat4()
            * glm::scale(glm::vec3(1.0f / scale, 1.0f / (scale * window_state.aspect_ratio), 1));
        window_state.mouse_pos_world = view_inverse * screen_coords;
    }

    if (window_state.mouse_active && game_state.mouse_mode.raw_mouse_motion) {
        glm::vec2 centre = {
            (float)window_state.screen_width / 2,
            (float)window_state.screen_height / 2
        };
        glfwSetCursorPos(window, centre.x, centre.y);
        window_state.mouse_delta = window_state.mouse_pos_screen - centre;
        window_state.mouse_pos_screen = centre;
    } else {
        window_state.mouse_delta = { 0, 0 };
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(bg.x, bg.y, bg.z, bg.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
