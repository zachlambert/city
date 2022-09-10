#pragma once

#include "component/agent.h"
#include "state/clock.h"
#include "system/mesh_renderer.h"


class AgentHandler {
public:
    struct Args {};
    AgentHandler(
        AgentList& agents,
        const Clock& clock,
        MeshRenderer& mesh_renderer,
        const Args& args);
    void tick();

private:
    AgentList& agents;
    const Clock& clock;
    MeshRenderer& mesh_renderer;

    int player_agent;
};
