#pragma once

#include "state/camera.h"
#include "state/input.h"
#include "state/clock.h"


class CameraHandler {
public:
    struct Args {};
    CameraHandler(
        Camera& camera,
        const Input& input,
        const Clock& clock,
        const Args& args);
    void tick();

private:
    Camera& camera;
    const Input& input;
    const Clock& clock;
};
