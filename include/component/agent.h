#pragma once

#include "component/component.h"
#include "maths/geometry.h"


struct Agent: public Component {
    struct Args {
        float body_size;
        float hand_size;
    };

    Spatial twist_target;
    glm::mat3 follow_gain;
    glm::mat3 twist_gain;
    float hand_separation;
    float hand_swing_freq;
    float hand_swing_phase;
    float hand_swing_amplitude;

    int body_rigid_body;
    int body_circle;
    int hand_left;
    int hand_right;
};
