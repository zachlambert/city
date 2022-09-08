
#include "system/game.h"
#include "system/mesh_renderer.h"
#include "system/physics.h"
#include "system/window.h"


struct State {
    WindowState window_state;

    MeshList meshes;
    MeshRenderer mesh_renderer;

    RigidBodyList rigid_bodies;
    AgentList agents;

    Game game;
    Window window;
    Physics physics;

    GameState game_state;

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
    Game::Args game_args;

    Window::Args window_args;
    window_args.width = 1200;
    window_args.height = 800;
    window_args.bg = glm::vec4(0.6, 0.6, 0.6, 1);
    window_args.fullscreen = false;
    window_args.name = "City";

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
