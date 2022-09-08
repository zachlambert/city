#pragma once

#include "state/game_state.h"
#include "state/window_state.h"
#include "component/agent.h"
#include "system/mesh_renderer.h"


class Game {
public:
    struct Args {};
    Game(
        GameState& game_state,
        const WindowState& window_state,
        MeshRenderer& mesh_renderer,
        AgentList& agents,
        const Args& args);
    void tick();

private:
    GameState& game_state;
    const WindowState& window_state;
    MeshRenderer& mesh_renderer;
    AgentList& agents;
};
