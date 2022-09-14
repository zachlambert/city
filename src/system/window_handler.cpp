
#include "system/window_handler.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <iostream>


Input* g_input = nullptr;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (!g_input) return;

    if (action == GLFW_PRESS) {
        g_input->key_states[key].just_changed = true;
        g_input->key_states[key].down = true;
    } else if (action == GLFW_RELEASE) {
        g_input->key_states[key].just_changed = true;
        g_input->key_states[key].down = false;
    }
}

void char_callback(GLFWwindow* window, unsigned int codepoint)
{
    if (!g_input) return;
    g_input->char_input = codepoint;
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!g_input) return;
    g_input->mouse_pos_screen = { xpos, ypos };
    g_input->mouse_active = true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (!g_input) return;
    KeyState* key_state = nullptr;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        key_state = &g_input->mouse_left_state;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        key_state = &g_input->mouse_right_state;
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

WindowHandler::WindowHandler(
    Viewport& viewport,
    Input& input,
    const Camera& camera,
    const Args& args
):
    viewport(viewport),
    input(input),
    camera(camera),
    window(nullptr),
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

    viewport.screen_width = args.width;
    viewport.screen_height = args.height;
    viewport.aspect_ratio = (float)args.width / args.height;

    input.mouse_active = false;
    input.mouse_pos_screen = { 0, 0 };
    input.mouse_pos_world = { 0, 0 };
    input.char_input = 0;
    for (const auto& key: args.keys) {
        input.key_states.emplace(key, KeyState());
    }

    viewport.view_active = true;
    viewport.open = true;

    g_input = &input;
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}

WindowHandler::~WindowHandler()
{
    g_input = nullptr;
    glfwTerminate();
}

void WindowHandler::tick()
{
    viewport.open = !glfwWindowShouldClose(window);
    glfwSwapBuffers(window);

    glfwSetInputMode(window, GLFW_CURSOR,
        camera.cursor_mode.cursor_hidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, 
        camera.cursor_mode.raw_mouse_motion ? GLFW_TRUE : GLFW_FALSE);

    input.char_input = 0;
    glfwPollEvents();

    viewport.view_matrix = camera.pose.inverse().to_mat4();
    const float scale = camera.zoom / camera.nominal_view_size;
    viewport.projection_matrix = glm::perspective(
        (float)M_PI/4, viewport.aspect_ratio, 1.f, 10000.0f
    ) * glm::mat4(coord_system_fix());

    {
        // Convert mouse pos to screen coords, ie: -1 -> 1
        glm::vec4 screen_coords(0.0f, 0.0f, 0.0f, 1.0f);
        screen_coords.x = 2 * (
            input.mouse_pos_screen.x
                - (float)viewport.screen_width / 2
            ) / viewport.screen_width;
        screen_coords.y = - 2 * (
            input.mouse_pos_screen.y
                - (float)viewport.screen_height / 2
            ) / viewport.screen_height;

        // Project onto world
        glm::mat4 view_inverse =
            camera.pose.to_mat4()
            * glm::scale(glm::vec3(1.0f / scale, 1.0f / (scale * viewport.aspect_ratio), 1));
        input.mouse_pos_world = view_inverse * screen_coords;
    }

    if (input.mouse_active && camera.cursor_mode.raw_mouse_motion) {
        glm::vec2 centre = {
            (float)viewport.screen_width / 2,
            (float)viewport.screen_height / 2
        };
        glfwSetCursorPos(window, centre.x, centre.y);
        input.mouse_delta = input.mouse_pos_screen - centre;
        input.mouse_pos_screen = centre;
    } else {
        input.mouse_delta = { 0, 0 };
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
