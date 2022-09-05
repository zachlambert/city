
#include "state.h"


int State::create_agent()
{
    int index = agents.create();

    Agent& agent = agents[index];
    agent.pos = { 0, 0 };
    agent.orient = 0;
    agent.body_shape = create_shape();
    agent.hand_shape_left = create_shape();
    agent.hand_shape_right = create_shape();

    return index;
}

void State::remove_agent(int index)
{
    const Agent& agent = agents[index];

    remove_shape(agent.hand_shape_right);
    remove_shape(agent.hand_shape_left);
    remove_shape(agent.body_shape);

    agents.remove(index);
}

void State::update_agents()
{
    for (int i = 0; i < agents.size(); i++) {
        Agent& agent = agents[i];
        if (!agent.valid) continue;

        agent.pos.x += 1 * dt;
        agent.orient += 0.2 * dt;
        shapes[agent.body_shape].pos = agent.pos;
        shapes[agent.hand_shape_left].pos = agent.pos
            + 0.5f * glm::vec2(cos(agent.orient + M_PI/2), sin(agent.orient + M_PI/2));
        shapes[agent.hand_shape_right].pos = agent.pos
            + 0.5f * glm::vec2(cos(agent.orient - M_PI/2), sin(agent.orient - M_PI/2));

        if (agent.pos.x > 5) {
            agents.remove(i);
        }
    }
}

