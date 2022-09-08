
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

    agent.twist_gain_lin = args.twist_gain_lin;
    agent.twist_gain_ang = args.twist_gain_ang;

    agent.twist_target.lin = glm::zero<glm::vec3>();
    agent.twist_target.ang = glm::zero<glm::vec3>();

    {
        RigidBody::Args rigid_body;
        rigid_body.initial_pose = args.initial_pose;
        rigid_body.mass = args.density * (4.0 / 3) * M_PI * pow(args.size / 2, 3);
        rigid_body.inertia =
            glm::identity<glm::mat3>()
            * rigid_body.mass * 0.4f * std::pow(args.size / 2, 2.0f);

        agent.rigid_body = create_rigid_body(
            agents.rigid_bodies,
            rigid_body
        );
    }

    {
        Mesh::Args mesh;
        mesh.mesh_id = agents.mesh_renderer.get_mesh("agent_body");
        mesh.scale = 1;

        agent.circle = create_mesh(
            agents.meshes,
            mesh
        );
    }

    return index;
}

void destroy_agent(
    AgentList& agents,
    MeshRenderer& mesh_renderer,
    int index)
{
    const Agent& agent = agents[index];

    destroy_rigid_body(agents.rigid_bodies, agent.rigid_body);
    destroy_mesh(agents.meshes, agent.circle);

    agents.remove(index);
}
