#pragma once

#include "component/component.h"


struct Clock: public Component {
    float dt;
    float prev_time;
    float current_time;
};
