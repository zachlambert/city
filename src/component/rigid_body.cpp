
#include "component/rigid_body.h"
#include <glm/gtc/constants.hpp>


int create_rigid_body(
    RigidBodyList& rigid_bodies,
    const RigidBody::Args& args)
{
    int index = rigid_bodies.create();
    RigidBody& rigid_body = rigid_bodies[index];

    rigid_body.inv_mass_matrix = {
        1.0 / args.mass, 0, 0,
        0, 1.0 / args.mass, 0,
        0, 0, 1.0 / args.moment_of_inertia
    };

    rigid_body.pose = args.initial_pose;
    rigid_body.twist = glm::zero<glm::vec3>();
    rigid_body.wrench = glm::zero<glm::vec3>();

    return index;
}

void destroy_rigid_body(
    RigidBodyList& rigid_bodies,
    int index)
{
    rigid_bodies.remove(index);
}
