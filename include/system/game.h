#pragma once

#include "component/component.h"

class Game {
public:
    bool init()
};
struct Game: public Component {
    int player_agent;
};
