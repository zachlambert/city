
#include "system/circle_renderer.h"
#include "system/game.h"
#include "system/physics.h"
#include "system/window.h"


struct State {
    RigidBodyList rigid_bodies;
    CircleList circles;
    AgentList agents;

    GameState game_state;
    WindowState window_state;

    State():
        agents(circles, rigid_bodies)
    {}
};


struct Systems {
    Game game;
    Window window;
    Physics physics;
    CircleRenderer circle_renderer;

    Systems(
        State& state,
        const Game::Args& game_args,
        const Window::Args& window_args,
        const Physics::Args& physics_args,
        const CircleRenderer::Args& circle_renderer_args
    ):
        game(state.game_state, state.window_state, state.agents, game_args),
        window(state.window_state, state.game_state, window_args),
        physics(state.game_state, state.rigid_bodies, physics_args),
        circle_renderer(state.window_state, state.circles, circle_renderer_args)
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

    CircleRenderer::Args circle_renderer_args;
    circle_renderer_args.shader_path = "shaders/";

    State state;
    Systems systems(
        state,
        game_args,
        window_args,
        physics_args,
        circle_renderer_args
    );

    while (state.window_state.running) {
        systems.game.tick();
        systems.physics.tick();
        systems.window.tick();
        systems.circle_renderer.tick();
    }
    return 0;
}
