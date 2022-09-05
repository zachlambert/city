
#include "state.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


int State::create_camera(Camera::Args args)
{
    int index = cameras.create();

    Camera& camera = cameras[index];
    camera.pos = { 0, 0 };
    camera.zoom = 1;
    camera.priority = 0;
    camera.valid = true;
    camera.agent = args.agent;

    return index;
}

void State::remove_camera(int index)
{
    const Camera& camera = cameras[index];

    cameras.remove(index);
}

void State::update_cameras()
{
    int max_priority = 0;
    for (int i = 0; i < cameras.size(); i++) {
        Camera& camera = cameras[i];
        if (!camera.valid) continue;

        if (camera.agent != -1)
        {
            const Agent& agent = agents[camera.agent];
            if (agent.valid) {
                camera.pos = agent.pos;
            } else {
                cameras.remove(i);
                continue;
            }
        }

        max_priority = std::max(camera.priority, max_priority);
    }

    for (int i = 0; i < cameras.size(); i++) {
        Camera& camera = cameras[i];
        if (!camera.valid) continue;
        if (camera.priority < max_priority) continue;

        static constexpr double base_scaling = 0.001f;
        window.view_matrix = glm::scale(glm::vec3(camera.zoom, base_scaling * camera.zoom * aspect_ratio, 1.0f / num_levels))
            * glm::translate(glm::vec3(-camera.pos.x, -camera.pos.y, 0));

        //  Get mouse pos on screen
        double mx, my;
        glfwGetCursorPos(window.window, &mx, &my);

        // Convert to screen coords, ie: -1 -> 1
        glm::vec4 screen_coords(0.0f, 0.0f, 0.0f, 1.0f);
        screen_coords.x = 2 * (mx - (double)window.width / 2) / window.width;
        screen_coords.y = - 2 * (my - (double)window.height / 2) / window.height;

        // Project onto world
        glm::mat4 view_inverse =
            glm::translate(glm::vec3(camera.pos.x, camera.pos.y, 0))
            * glm::scale(glm::vec3(1.0f / (base_scaling * camera.zoom), 1.0f / (base_scaling * camera.zoom * window.aspect_ratio), 1));
        window.mouse_pos = view_inverse * screen_coords;

        window.have_world_view = true;

        return;
    }

    window.have_world_view = false;
}
