#pragma once

#include "maths/geometry.h"
#include <variant>
#include "state/input_state.h"


struct CameraModeTopDown {
    glm::vec3 pos;
    float yaw;
};
struct CameraModeFirstPerson {
    glm::vec3 pos;
    glm::vec3 euler;
};

typedef std::variant<
    CameraModeTopDown,
    CameraModeFirstPerson
> CameraMode;

struct GameState {
    float dt;
    float prev_time;
    float current_time;
    int player_agent;
    struct {
        CameraMode mode;
        Pose pose;
        float zoom;
        float nominal_view_size;
    } camera;
    MouseMode mouse_mode;
};

