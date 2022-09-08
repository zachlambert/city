
#include "system/game.h"
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>


Game::Game(
    GameState& game_state,
    const WindowState& window_state,
    AgentList& agents,
    const Args& args
):
    game_state(game_state),
    window_state(window_state),
    agents(agents)
{
    Agent::Args default_agent;
    {
        default_agent.initial_pose = { 0, 0, 0 };
        default_agent.size = 0.5;
        default_agent.density = 1000;
        default_agent.color = { 0, 0.5, 1.0, 1 };
        default_agent.follow_gain_lin = 1000;
        default_agent.follow_gain_ang = 0;
        default_agent.twist_gain_lin = 1000;
        default_agent.twist_gain_ang = 0;
        game_state.player_agent = create_agent(agents, default_agent);
    }

    for (size_t i = 0; i < 200000; i++) {
        Agent::Args agent = default_agent;
        agent.initial_pose.x = -5 + 10 * (float)rand() / (float)RAND_MAX;
        agent.initial_pose.y = -5 + 10 * (float)rand() / (float)RAND_MAX;
        create_agent(agents, agent);
    }

    game_state.camera.pose = agents.rigid_bodies[agents[game_state.player_agent].rigid_body].pose;
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

#if 0
    if (game_state.player_agent < agents.size() && agents[game_state.player_agent].valid) {
        game_state.camera.pose = agents.rigid_bodies[agents[game_state.player_agent].rigid_body].pose;
        game_state.camera.pose.theta() = 0;
    }
#endif

    for (int i = 0; i < agents.size(); i++) {
        Agent& agent = agents[i];
        if (!agent.valid) continue;

        RigidBody& rigid_body = agents.rigid_bodies[agent.rigid_body];

        Pose pose_error(0, 0, 0);
        if (window_state.mouse_active) {
            pose_error.pos() = window_state.mouse_pos_world - rigid_body.pose.pos();
            pose_error.theta() = clamp_angle(std::atan2(pose_error.pos().y, pose_error.pos().x) - rigid_body.pose.theta());
        }

        if (glm::length(pose_error.pos()) < 0.01) {
            agent.twist_target = glm::zero<glm::vec3>();
        } else {
            agent.twist_target = agent.follow_gain * pose_error;
            float lin_speed = glm::length(agent.twist_target.lin());
            float ang_speed = std::fabs(agent.twist_target.ang());
            if (lin_speed > 5.0) {
                agent.twist_target.lin() *= (5.0f / lin_speed);
            }
            if (ang_speed > 5.0) {
                agent.twist_target.ang() *= (5.0f / ang_speed);
            }
        }

        rigid_body.wrench = agent.twist_gain * (agent.twist_target - rigid_body.twist);

        Circle& circle = agents.circles[agent.circle];
        circle.pos = rigid_body.pose.pos();
    }
}
