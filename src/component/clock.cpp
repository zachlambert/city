
#include "state.h"


void State::create_clock()
{
    assert(!clock.valid);

    clock.prev_time = glfwGetTime();
    clock.dt = 0;
    clock.current_time = clock.prev_time;
    clock.valid = true;
}

void State::remove_clock()
{
    assert(clock.valid);
    clock.valid = false;
}

void State::update_clock()
{
    clock.current_time = glfwGetTime();
    clock.dt = clock.current_time - clock.prev_time;
    clock.prev_time = clock.current_time;
}
