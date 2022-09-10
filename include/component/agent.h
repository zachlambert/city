#pragma once

#include "component/component.h"
#include "maths/geometry.h"

#include "component/rigid_body.h"
#include "component/mesh.h"

#include "system/mesh_renderer.h"


struct Agent {
    struct Args {
        Pose initial_pose;
        float size;
        float density;
        glm::vec4 color;
        struct {
            int head;
            int body;
        } meshes;
        float twist_gain_lin;
        float twist_gain_ang;
    };

    Spatial twist_target;
    float twist_gain_lin;
    float twist_gain_ang;

    int rigid_body;
    struct {
        int body;
        int head;
    } meshes;
    bool valid;
};
class AgentList: public ComponentList<Agent> {
public:
    AgentList(
        MeshList& meshes,
        RigidBodyList& rigid_bodies
    ):
        meshes(meshes),
        rigid_bodies(rigid_bodies)
    {}
    MeshList& meshes;
    RigidBodyList& rigid_bodies;
};

int create_agent(
    AgentList& agents,
    const Agent::Args& args
);

void destroy_agent(
    AgentList& agents,
    int index
);
