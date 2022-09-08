#pragma once

#include "maths/geometry.h"


struct GameState {
    float dt;
    float prev_time;
    float current_time;
    int player_agent;
    struct {
        Pose pose;
        float zoom;
        float nominal_view_size;
    } camera;
};

