
#include "system/game.h"
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>


Game::Game(
    GameState& game_state,
    const WindowState& window_state,
    MeshRenderer& mesh_renderer,
    AgentList& agents,
    const Args& args
):
    game_state(game_state),
    window_state(window_state),
    mesh_renderer(mesh_renderer),
    agents(agents)
{
    {
        std::vector<MeshVertex> vertices;
        std::vector<unsigned short> indices;
        vertices.push_back({
            glm::vec3(0, -0.5, 0),
            glm::vec3(-1, 0, 0),
            glm::vec4(1, 0, 0, 1)
        });
        vertices.push_back({
            glm::vec3(0, -0.5, 1),
            glm::vec3(-1, 0, 0),
            glm::vec4(1, 0, 0, 1)
        });
        vertices.push_back({
            glm::vec3(0, 0.5, 1),
            glm::vec3(-1, 0, 0),
            glm::vec4(1, 0, 0, 1)
        });
        vertices.push_back({
            glm::vec3(0, 0.5, 0),
            glm::vec3(-1, 0, 0),
            glm::vec4(1, 0, 0, 1)
        });
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);
        vertices.push_back({
            glm::vec3(1, -0.5, 1),
            glm::vec3(0, 0, 1),
            glm::vec4(1, 0, 0, 1)
        });
        vertices.push_back({
            glm::vec3(0, -0.5, 1),
            glm::vec3(0, 0, 1),
            glm::vec4(1, 0, 0, 1)
        });
        vertices.push_back({
            glm::vec3(0, 0.5, 1),
            glm::vec3(0, 0, 1),
            glm::vec4(1, 0, 0, 1)
        });
        vertices.push_back({
            glm::vec3(1, 0.5, 1),
            glm::vec3(0, 0, 1),
            glm::vec4(1, 0, 0, 1)
        });
        indices.push_back(6);
        indices.push_back(5);
        indices.push_back(4);
        indices.push_back(4);
        indices.push_back(7);
        indices.push_back(6);
        mesh_renderer.load_mesh("agent_body", vertices, indices);
    }

    Agent::Args default_agent;
    {
        default_agent.initial_pose = Pose::identity();
        default_agent.size = 0.5;
        default_agent.density = 1000;
        default_agent.color = { 0, 0.5, 1.0, 1 };
        default_agent.twist_gain_lin = 1000;
        default_agent.twist_gain_ang = 100;
        game_state.player_agent = create_agent(agents, default_agent);
    }

#if 0
    for (size_t i = 0; i < 200000; i++) {
        Agent::Args agent = default_agent;
        agent.initial_pose.pos.x = -5 + 10 * (float)rand() / (float)RAND_MAX;
        agent.initial_pose.pos.y = -5 + 10 * (float)rand() / (float)RAND_MAX;
        create_agent(agents, agent);
    }
#endif

    {
        CameraModeFirstPerson mode;
        mode.pos = glm::vec3(-5, 0, 3);
        mode.euler = glm::vec3(0, M_PI/4, 0);
        game_state.camera.mode = mode;
    }
    game_state.camera.zoom = 1;
    game_state.camera.nominal_view_size = 10;

    game_state.prev_time = glfwGetTime();
    game_state.current_time = glfwGetTime();
    game_state.dt = 0;
}

void Game::tick()
{
    game_state.prev_time = game_state.current_time;
    game_state.current_time = glfwGetTime();
    game_state.dt = game_state.current_time - game_state.prev_time;

    if (window_state.key_states.at(GLFW_KEY_1).pressed()) {
        if (auto prior = std::get_if<CameraModeTopDown>(&game_state.camera.mode)) {
            CameraModeFirstPerson mode;
            mode.pos = prior->pos;
            mode.euler = glm::vec3(0, 0.49 * M_PI, prior->yaw);
            game_state.camera.mode = mode;
        }
    }
    if (window_state.key_states.at(GLFW_KEY_2).pressed()) {
        if (auto prior = std::get_if<CameraModeFirstPerson>(&game_state.camera.mode)) {
            CameraModeTopDown mode;
            mode.pos = game_state.camera.pose.pos;
            
            // Centre on the point on the floor at the cursor if nearby
            float dist_to_floor = prior->pos.z / sin(prior->euler.y);
            if (dist_to_floor > 0 && dist_to_floor < 20) {
                mode.pos += game_state.camera.pose.orient * glm::vec3(1, 0, 0) * dist_to_floor;
            }

            mode.pos.z = 5;
            mode.yaw = prior->euler.z;
            game_state.camera.mode = mode;
        }
    }

    if (auto mode = std::get_if<CameraModeFirstPerson>(&game_state.camera.mode)) {

        glm::vec3 vel = glm::zero<glm::vec3>();
        if (window_state.key_states.at(GLFW_KEY_D).down) vel.y -= 5;
        if (window_state.key_states.at(GLFW_KEY_A).down) vel.y += 5;
        if (window_state.key_states.at(GLFW_KEY_S).down) vel.x -= 5;
        if (window_state.key_states.at(GLFW_KEY_W).down) vel.x += 5;
        if (window_state.key_states.at(GLFW_KEY_LEFT_SHIFT).down) vel.z -= 5;
        if (window_state.key_states.at(GLFW_KEY_SPACE).down) vel.z += 5;
        mode->pos += game_state.camera.pose.orient * vel * game_state.dt;

        glm::vec3 euler_dot = glm::zero<glm::vec3>();
        euler_dot.z = -window_state.mouse_delta.x;
        euler_dot.y = window_state.mouse_delta.y;

        mode->euler += euler_dot * game_state.dt;
        mode->euler.x = 0;
        constexpr float pitch_limit = (float)M_PI * 0.49;
        mode->euler.y = std::clamp(mode->euler.y, -pitch_limit, pitch_limit);
        mode->euler.z = clamp_angle(mode->euler.z);

        glm::mat3 p_rot = glm::mat4(glm::mat3(
            glm::vec3(0, 0, -1),
            glm::vec3(-1, 0, 0),
            glm::vec3(0, 1, 0)
        ));

        game_state.camera.pose.pos = mode->pos;
        game_state.camera.pose.orient = glm::orientate3(glm::vec3(
            -mode->euler.z, mode->euler.y, mode->euler.x
        ));
        game_state.camera.pose.orient =
            coord_system_fix()
            * game_state.camera.pose.orient
            * glm::transpose(coord_system_fix());
        
        game_state.mouse_mode.raw_mouse_motion = true;
        game_state.mouse_mode.cursor_hidden = true;

    } else if (auto mode = std::get_if<CameraModeTopDown>(&game_state.camera.mode)) {

        glm::vec3 vel = glm::zero<glm::vec3>();
        if (window_state.key_states.at(GLFW_KEY_D).down) vel.y -= 5;
        if (window_state.key_states.at(GLFW_KEY_A).down) vel.y += 5;
        if (window_state.key_states.at(GLFW_KEY_S).down) vel.z -= 5;
        if (window_state.key_states.at(GLFW_KEY_W).down) vel.z += 5;
        if (window_state.key_states.at(GLFW_KEY_LEFT_SHIFT).down) vel.x += 5;
        if (window_state.key_states.at(GLFW_KEY_SPACE).down) vel.x -= 5;
        mode->pos += game_state.camera.pose.orient * vel * game_state.dt;

        float yaw_dot = 0;
        if (window_state.key_states.at(GLFW_KEY_E).down) yaw_dot -= 3;
        if (window_state.key_states.at(GLFW_KEY_Q).down) yaw_dot += 3;
        mode->yaw += yaw_dot * game_state.dt;
        mode->yaw = clamp_angle(mode->yaw);

        game_state.camera.pose.pos = mode->pos;
        game_state.camera.pose.orient = glm::mat3(
            glm::vec3(0, 0, -1),
            glm::vec3(-sin(mode->yaw), cos(mode->yaw), 0),
            glm::vec3(cos(mode->yaw), sin(mode->yaw), 0)
        );

        game_state.mouse_mode.raw_mouse_motion = false;
        game_state.mouse_mode.cursor_hidden = false;
    }

    for (int i = 0; i < agents.size(); i++) {
        Agent& agent = agents[i];
        if (!agent.valid) continue;

        RigidBody& rigid_body = agents.rigid_bodies[agent.rigid_body];

        rigid_body.wrench.lin = agent.twist_gain_lin
            * (agent.twist_target.lin - rigid_body.twist.lin);
        rigid_body.wrench.ang = agent.twist_gain_ang
            * (agent.twist_target.ang - rigid_body.twist.ang);

        Mesh& mesh = agents.meshes[agent.mesh];
        mesh.pose = rigid_body.pose;
    }
}
