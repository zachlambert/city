
#include "system/game.h"
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>


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

    for (size_t i = 0; i < 200000; i++) {
        Agent::Args agent = default_agent;
        agent.initial_pose.pos.x = -5 + 10 * (float)rand() / (float)RAND_MAX;
        agent.initial_pose.pos.y = -5 + 10 * (float)rand() / (float)RAND_MAX;
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

    for (int i = 0; i < agents.size(); i++) {
        Agent& agent = agents[i];
        if (!agent.valid) continue;

        RigidBody& rigid_body = agents.rigid_bodies[agent.rigid_body];

        rigid_body.wrench.lin = agent.twist_gain_lin
            * (agent.twist_target.lin - rigid_body.twist.lin);
        rigid_body.wrench.ang = agent.twist_gain_ang
            * (agent.twist_target.ang - rigid_body.twist.ang);

        Mesh& mesh = agents.meshes[agent.circle];
        mesh.pose = rigid_body.pose;
    }
}
