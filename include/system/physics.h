
#pragma once

#include "system/game.h"
#include "component/rigid_body.h"

class Physics {
public:
    struct Args {};
    Physics(
        const GameState& game_state,
        RigidBodyList& rigid_bodies,
        const Args& args);
    void tick();

private:
    const GameState& game_state;
    RigidBodyList& rigid_bodies;
};
