
#include "state.h"
#include <thread>
#include <GL/glew.h>
#include <glm/glm.hpp>

void State::init()
{
    create_window();
    create_game();
}

void State::deinit()
{
    remove_game();
    remove_window();
}

void State::update()
{
    update_game();
    update_window();
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
