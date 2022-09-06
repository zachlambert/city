#pragma one

#include "component/component.h"
#include "maths/geometry.h"


struct RigidBody: public Component {
    struct Args {
        Pose initial_pose;
        float mass;
        float moment_of_inertia;
    };
    Pose pose;
    Spatial twist;
    Spatial wrench;
    glm::mat3 inv_mass_matrix;
};

