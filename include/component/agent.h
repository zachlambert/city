#pragma once

#include "component/component.h"
#include "maths/geometry.h"

#include "component/rigid_body.h"
#include "component/circle.h"


struct Agent {
    struct Args {
        Pose initial_pose = Pose(0, 0, 0);
        float size = 0;
        float density = 0;
        glm::vec4 color = glm::vec4(0, 0, 0, 0);
        float follow_gain_lin = 0;
        float follow_gain_ang = 0;
        float twist_gain_lin = 0;
        float twist_gain_ang = 0;
    };

    Spatial twist_target;
    glm::mat3 follow_gain;
    glm::mat3 twist_gain;

    int rigid_body;
    int circle;
    bool valid;
};
class AgentList: public ComponentList<Agent> {
public:
    AgentList(
        CircleList& circles,
        RigidBodyList& rigid_bodies
    ):
        circles(circles),
        rigid_bodies(rigid_bodies)
    {}
    CircleList& circles;
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
