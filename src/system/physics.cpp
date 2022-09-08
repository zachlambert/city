
#include "system/physics.h"


Physics::Physics(
    const GameState& game_state,
    ComponentList<RigidBody>& rigid_bodies,
    const Args& args
):
    game_state(game_state),
    rigid_bodies(rigid_bodies)
{}

void Physics::tick()
{
    for (int i = 0; i < rigid_bodies.size(); i++) {
        RigidBody& rigid_body = rigid_bodies[i];
        if (!rigid_body.valid) continue;

        Spatial acc = rigid_body.inv_inertia * (
            rigid_body.wrench
            + adjoint_map_momentum(
                rigid_body.twist,
                rigid_body.inertia * rigid_body.twist));

        float dt = game_state.dt;
        rigid_body.pose = rigid_body.pose * matrix_exp(dt * rigid_body.twist + 0.5 * pow(dt, 2) * acc);
        rigid_body.twist += acc * dt;
    }
}
