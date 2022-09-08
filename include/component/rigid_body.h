#pragma once

#include "component/component.h"
#include "maths/geometry.h"


struct RigidBody {
    struct Args {
        Pose initial_pose;
        float mass;
        glm::mat3 inertia;
    };

    Pose pose;
    Spatial twist;
    Spatial wrench;
    SpatialInertia inertia;
    SpatialInertia inv_inertia;
    bool valid;
};
typedef ComponentList<RigidBody> RigidBodyList;

int create_rigid_body(
    RigidBodyList& rigid_bodies,
    const RigidBody::Args& args
);

void destroy_rigid_body(
    RigidBodyList& rigid_body,
    int index
);
