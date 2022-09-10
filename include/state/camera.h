#pragma once

#include <glm/glm.hpp>
#include <variant>
#include "maths/geometry.h"


struct CameraModeTopDown {
    glm::vec3 pos;
    float yaw;
};
struct CameraModeFirstPerson {
    glm::vec3 pos;
    glm::vec3 euler;
    bool free;
};

typedef std::variant<
    CameraModeTopDown,
    CameraModeFirstPerson
> CameraMode;

struct CursorMode {
    bool cursor_hidden;
    bool raw_mouse_motion;
    CursorMode():
        cursor_hidden(false),
        raw_mouse_motion(false)
    {}
};

struct Camera {
    CameraMode mode;
    CursorMode cursor_mode;
    Pose pose;
    float zoom;
    float nominal_view_size;
};
