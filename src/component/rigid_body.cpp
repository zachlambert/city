
#include "component/rigid_body.h"
#include <glm/gtc/constants.hpp>


int create_rigid_body(
    RigidBodyList& rigid_bodies,
    const RigidBody::Args& args)
{
    int index = rigid_bodies.create();
    RigidBody& rigid_body = rigid_bodies[index];

    rigid_body.inertia.mass = args.mass;
    rigid_body.inertia.inertia = args.inertia;
    rigid_body.inv_inertia.mass = 1.0f / args.mass;
    rigid_body.inv_inertia.inertia = glm::inverse(args.inertia);

    rigid_body.pose = args.initial_pose;
    rigid_body.twist = Spatial::zero();
    rigid_body.wrench = Spatial::zero();

    return index;
}

void destroy_rigid_body(
    RigidBodyList& rigid_bodies,
    int index)
{
    rigid_bodies.remove(index);
}
