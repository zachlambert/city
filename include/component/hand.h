#pragma once

#include "component/component.h"
#include "maths/geometry.h"


struct Hand: public Component {
    struct Args {
        float size;
        Pose initial_pose;
        float gain_lin;
        float gain_ang;
        float damping_lin;
        float damping_ang;
    };
    Pose pose_target;
    glm::mat3 gain;
    glm::mat3 damping;

    int rigid_body;
    int circle;
};
