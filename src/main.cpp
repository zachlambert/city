
#include "system/game.h"
#include "system/mesh_renderer.h"
#include "system/physics.h"
#include "system/window.h"
#include <yaml-cpp/yaml.h>


struct State {
    WindowState window_state;
    GameState game_state;

    Window window;

    MeshList meshes;
    MeshRenderer mesh_renderer;

    RigidBodyList rigid_bodies;
    AgentList agents;

    Game game;
    Physics physics;

    State(
        const Game::Args& game_args,
        const Window::Args& window_args,
        const Physics::Args& physics_args,
        const MeshRenderer::Args& mesh_renderer_args
    ):
        mesh_renderer(window_state, meshes, mesh_renderer_args),
        agents(meshes, rigid_bodies, mesh_renderer),
        game(game_state, window_state, mesh_renderer, agents, game_args),
        window(window_state, game_state, window_args),
        physics(game_state, rigid_bodies, physics_args)
    {}
};

int main()
{
    auto config = YAML::LoadFile("config.yaml");

    Game::Args game_args;

    Window::Args window_args;
    {
        auto node = config["window"];
        window_args.width = node["width"].as<size_t>();
        window_args.height = node["height"].as<size_t>();
        window_args.bg = glm::vec4(0.6, 0.6, 0.6, 1);
        window_args.fullscreen = false;
        window_args.name = "City";
        window_args.keys = {
            GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_S, GLFW_KEY_W,
            GLFW_KEY_LEFT_SHIFT, GLFW_KEY_SPACE,
            GLFW_KEY_Q, GLFW_KEY_E,
            GLFW_KEY_1, GLFW_KEY_2
        };
    }

    Physics::Args physics_args;

    MeshRenderer::Args mesh_renderer_args;
    mesh_renderer_args.shader_path = "shaders/";

    State state(
        game_args,
        window_args,
        physics_args,
        mesh_renderer_args
    );

    while (state.window_state.running) {
        state.game.tick();
        state.physics.tick();
        state.window.tick();
        state.mesh_renderer.tick();
    }
    return 0;
}
