
#include "system/agent_handler.h"
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>


AgentHandler::AgentHandler(
    AgentList& agents,
    const Clock& clock,
    MeshRenderer& mesh_renderer,
    const Args& args
):
    agents(agents),
    clock(clock),
    mesh_renderer(mesh_renderer),
    player_agent(-1)
{
    {
        std::vector<MeshVertex> vertices;
        std::vector<unsigned short> indices;
        glm::vec4 color = glm::vec4(0, 0.5, 1, 1);

        generate_box(vertices, indices, color, glm::vec3(0.4, 0.2, 0.5));
        mesh_renderer.load_mesh("agent_head", vertices, indices);

        generate_capsule(vertices, indices, color, 0.3, 1, 0.05);
        mesh_renderer.load_mesh("agent_body", vertices, indices);
    }

    Agent::Args default_agent;
    {
        default_agent.initial_pose = Pose::identity();
        default_agent.initial_pose.pos.z += 0.8;

        default_agent.size = 0.5;
        default_agent.density = 1000;
        default_agent.color = { 0, 0.5, 1.0, 1 };

        default_agent.meshes.head = mesh_renderer.get_mesh("agent_head");
        default_agent.meshes.body = mesh_renderer.get_mesh("agent_body");

        default_agent.twist_gain_lin = 1000;
        default_agent.twist_gain_ang = 100;

        player_agent = create_agent(agents, default_agent);
    }

#if 0
    for (size_t i = 0; i < 5000; i++) {
        Agent::Args agent = default_agent;
        agent.initial_pose.pos.x = -20 + 40 * (float)rand() / (float)RAND_MAX;
        agent.initial_pose.pos.y = -20 + 40 * (float)rand() / (float)RAND_MAX;
        agent.initial_pose.orient = euler_to_rotation(glm::vec3(0, 0,
            -M_PI + (float)rand() / (float)(2*M_PI)
        ));
        create_agent(agents, agent);
    }
#endif
}

void AgentHandler::tick()
{
    for (int i = 0; i < agents.size(); i++) {
        Agent& agent = agents[i];
        if (!agent.valid) continue;

        RigidBody& rigid_body = agents.rigid_bodies[agent.rigid_body];

        rigid_body.wrench.lin = agent.twist_gain_lin
            * (agent.twist_target.lin - rigid_body.twist.lin);
        rigid_body.wrench.ang = agent.twist_gain_ang
            * (agent.twist_target.ang - rigid_body.twist.ang);

        agents.meshes[agent.meshes.body].pose = rigid_body.pose;
        Pose pose;
        agents.meshes[agent.meshes.head].pose = rigid_body.pose * Pose{
            glm::vec3(0, 0, 1), glm::identity<glm::mat3>()
        };
    }
}
