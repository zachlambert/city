
#include "state.h"

void State::create_game()
{
    assert(!game.valid);

    Agent::Args args;
    args.body_size = 1;
    args.hand_size = 0.2;

    {
        game.player_agent = create_agent(args);
        {
            Camera::Args args;
            args.agent = game.player_agent;
            create_camera(args);
        }
    }
    // for (size_t i = 0; i < 10; i++) {
    //     create_agent(args);
    // }

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
