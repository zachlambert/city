
#include "system/clock_handler.h"
#include <GLFW/glfw3.h>


ClockHandler::ClockHandler(
    Clock& clock
):
    clock(clock)
{
    clock.prev_time = glfwGetTime();
    clock.current_time = glfwGetTime();
    clock.dt = 0;
}

void ClockHandler::tick()
{
    clock.prev_time = clock.current_time;
    clock.current_time = glfwGetTime();
    clock.dt = clock.current_time - clock.prev_time;
}
