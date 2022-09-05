
#include "state.h"

void State::create_game()
{
    assert(!game.valid);

    {
        game.player_agent = create_agent();
        {
            Camera::Args args;
            args.agent = game.player_agent;
            create_camera(args);
        }
    }

    game.valid = true;
}

void State::update_game()
{

}

void State::remove_game()
{
    assert(game.valid);

    // TODO: Clear agents, cameras, etc?

    game.valid = false;
}
