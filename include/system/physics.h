
#pragma once

#include "state/clock.h"
#include "component/rigid_body.h"

class Physics {
public:
    struct Args {};
    Physics(
        const Clock& clock,
        RigidBodyList& rigid_bodies,
        const Args& args);
    void tick();

private:
    const Clock& clock;
    RigidBodyList& rigid_bodies;
};
