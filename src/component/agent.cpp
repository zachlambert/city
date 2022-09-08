
#include "component/agent.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/constants.hpp>
#include <glm/gtx/matrix_operation.hpp>


int create_agent(
    AgentList& agents,
    const Agent::Args& args)
{
    int index = agents.create();
    Agent& agent = agents[index];

    agent.follow_gain = glm::diagonal3x3(glm::vec3(
        args.follow_gain_lin,
        args.follow_gain_lin,
        args.follow_gain_ang));
    agent.twist_gain = glm::diagonal3x3(glm::vec3(
        args.twist_gain_lin,
        args.twist_gain_lin,
        args.twist_gain_ang));

    agent.twist_target = glm::zero<glm::vec3>();

    float mass = args.density * (4.0 / 3) * M_PI * pow(args.size / 2, 3);
    agent.rigid_body = create_rigid_body(
        agents.rigid_bodies,
        {
            args.initial_pose,
            mass,
            mass * 0.4f * std::pow(args.size / 2, 2.0f)
        }
    );

    agent.circle = create_circle(
        agents.circles,
        {
            args.color,
            args.size / 2,
            0
        }
    );

    return index;
}

void destroy_agent(
    AgentList& agents,
    int index)
{
    const Agent& agent = agents[index];

    destroy_rigid_body(agents.rigid_bodies, agent.rigid_body);
    destroy_circle(agents.circles, agent.circle);

    agents.remove(index);
}
