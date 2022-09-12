
#include "system/camera_handler.h"
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>


CameraHandler::CameraHandler(
    Camera& camera,
    const Input& input,
    const Clock& clock,
    const Args& args
):
    camera(camera),
    input(input),
    clock(clock)
{
    {
        CameraModeFirstPerson mode;
        mode.pos = glm::vec3(-5, 0, 3);
        mode.euler = glm::vec3(0, M_PI/4, 0);
        mode.free = false;
        camera.mode = mode;
    }
    camera.zoom = 1;
    camera.nominal_view_size = 10;
}

void CameraHandler::tick()
{
    if (input.key_states.at(GLFW_KEY_1).pressed()) {
        if (auto prior = std::get_if<CameraModeTopDown>(&camera.mode)) {
            CameraModeFirstPerson mode;
            mode.pos = prior->pos;
            mode.euler = glm::vec3(0, 0.49 * M_PI, prior->yaw);
            camera.mode = mode;
        }
    }
    if (input.key_states.at(GLFW_KEY_2).pressed()) {
        if (auto prior = std::get_if<CameraModeFirstPerson>(&camera.mode)) {
            CameraModeTopDown mode;
            mode.pos = camera.pose.pos;
            
            // Centre on the point on the floor at the cursor if nearby
            float dist_to_floor = prior->pos.z / sin(prior->euler.y);
            if (dist_to_floor > 0 && dist_to_floor < 20) {
                mode.pos += camera.pose.orient * glm::vec3(1, 0, 0) * dist_to_floor;
            }

            mode.pos.z = 20;
            mode.yaw = prior->euler.z;
            camera.mode = mode;
        }
    }

    if (auto mode = std::get_if<CameraModeFirstPerson>(&camera.mode)) {

        glm::vec3 vel = glm::zero<glm::vec3>();
        if (input.key_states.at(GLFW_KEY_D).down) vel.y -= 20;
        if (input.key_states.at(GLFW_KEY_A).down) vel.y += 20;
        if (input.key_states.at(GLFW_KEY_S).down) vel.x -= 20;
        if (input.key_states.at(GLFW_KEY_W).down) vel.x += 20;
        if (input.key_states.at(GLFW_KEY_LEFT_SHIFT).down) vel.z -= 20;
        if (input.key_states.at(GLFW_KEY_SPACE).down) vel.z += 20;
        mode->pos += camera.pose.orient * vel * clock.dt;

        glm::vec3 euler_dot = glm::zero<glm::vec3>();
        euler_dot.z = -1e-3 * input.mouse_delta.x / clock.dt;
        euler_dot.y = 1e-3 * input.mouse_delta.y / clock.dt;

        mode->euler += euler_dot * clock.dt;
        mode->euler.x = 0;
        constexpr float pitch_limit = (float)M_PI * 0.49;
        mode->euler.y = std::clamp(mode->euler.y, -pitch_limit, pitch_limit);
        mode->euler.z = clamp_angle(mode->euler.z);

        glm::mat3 p_rot = glm::mat4(glm::mat3(
            glm::vec3(0, 0, -1),
            glm::vec3(-1, 0, 0),
            glm::vec3(0, 1, 0)
        ));

        camera.pose.pos = mode->pos;
        camera.pose.orient = glm::orientate3(glm::vec3(
            -mode->euler.z, mode->euler.y, mode->euler.x
        ));
        camera.pose.orient = euler_to_rotation(mode->euler);
            coord_system_fix()
            * camera.pose.orient
            * glm::transpose(coord_system_fix());
        
        if (input.mouse_right_state.pressed()) {
            mode->free = !mode->free;
        }
        
        if (!mode->free) {
            camera.cursor_mode.raw_mouse_motion = true;
            camera.cursor_mode.cursor_hidden = true;
        } else {
            camera.cursor_mode.raw_mouse_motion = false;
            camera.cursor_mode.cursor_hidden = false;
        }

    } else if (auto mode = std::get_if<CameraModeTopDown>(&camera.mode)) {

        glm::vec3 vel = glm::zero<glm::vec3>();
        if (input.key_states.at(GLFW_KEY_D).down) vel.y -= 20;
        if (input.key_states.at(GLFW_KEY_A).down) vel.y += 20;
        if (input.key_states.at(GLFW_KEY_S).down) vel.z -= 20;
        if (input.key_states.at(GLFW_KEY_W).down) vel.z += 20;
        if (input.key_states.at(GLFW_KEY_LEFT_SHIFT).down) vel.x += 20;
        if (input.key_states.at(GLFW_KEY_SPACE).down) vel.x -= 20;
        mode->pos += camera.pose.orient * vel * clock.dt;

        float yaw_dot = 0;
        if (input.key_states.at(GLFW_KEY_E).down) yaw_dot -= 3;
        if (input.key_states.at(GLFW_KEY_Q).down) yaw_dot += 3;
        mode->yaw += yaw_dot * clock.dt;
        mode->yaw = clamp_angle(mode->yaw);

        camera.pose.pos = mode->pos;
        camera.pose.orient = glm::mat3(
            glm::vec3(0, 0, -1),
            glm::vec3(-sin(mode->yaw), cos(mode->yaw), 0),
            glm::vec3(cos(mode->yaw), sin(mode->yaw), 0)
        );

        camera.cursor_mode.raw_mouse_motion = false;
        camera.cursor_mode.cursor_hidden = false;
    }
}
