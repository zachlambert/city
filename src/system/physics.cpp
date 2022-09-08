
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

        Spatial acc = rigid_body.inv_mass_matrix * rigid_body.wrench;
        float dt = game_state.dt;
        rigid_body.pose += rigid_body.twist * dt + acc * 0.5f * std::pow(dt, 2.0f);
        rigid_body.twist += acc * dt;
    }
}
