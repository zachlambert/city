
#include "state.h"
#include <thread>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>


void State::init()
{
    create_window();
    create_circle_renderer();
    create_game();
    create_clock();
}

void State::deinit()
{
    remove_window();
    remove_circle_renderer();
    remove_game();
    remove_clock();
}

void State::update()
{
    update_clock();
    update_game();
    update_agents();
    update_hands();
    update_rigid_bodies();
    update_cameras();

    update_circles();

    update_window();
    update_circle_renderer();
}

int main()
{
    State state;
    state.init();
    while (state.window.running) {
        state.update();
    }
    state.deinit();

    return 0;
}
