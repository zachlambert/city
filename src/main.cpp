
#include "state.h"
#include <thread>
#include <GL/glew.h>
#include <glm/glm.hpp>

void State::init()
{
    create_window();
    create_shape_renderer();
    create_game();
    create_clock();
}

void State::deinit()
{
    remove_window();
    remove_shape_renderer();
    remove_game();
    remove_clock();
}

void State::update()
{
    update_clock();
    update_game();
    update_agents();
    update_cameras();

    update_shapes();

    update_window();
    update_shape_renderer();
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
