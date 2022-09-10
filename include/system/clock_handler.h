#pragma once

#include "state/clock.h"


class ClockHandler {
public:
    struct Args {};
    ClockHandler(
        Clock& clock);
    void tick();

private:
    Clock& clock;
};
