
#include "state.h"
#include <iostream>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/string_cast.hpp>


static constexpr float density = 0.05f;

int State::create_agent(Agent::Args args_in)
{
    float x = -5 + 10 * ((float)rand() / (float)RAND_MAX);
    float y = -5 + 10 * ((float)rand() / (float)RAND_MAX);
    float theta = -M_PI + 2 * M_PI * ((float)rand() / (float)RAND_MAX);
    float speed = 1.0 * ((float)rand() / (float)RAND_MAX);
    glm::vec3 pose = { x, y, theta };

    int index = agents.create();
    Agent& agent = agents[index];
    agent.follow_gain = glm::diagonal3x3(glm::vec3(1.0, 1.0, 20.0));
    agent.twist_gain = glm::diagonal3x3(glm::vec3(1.0, 1.0, 1.0));
    agent.twist_target = glm::zero<glm::vec3>();
    agent.hand_separation = args_in.body_size * 1.2;
    agent.hand_swing_freq = 1.0;
    agent.hand_swing_phase = -M_PI + 2 * M_PI * (float)rand() / (float)RAND_MAX;
    agent.hand_swing_amplitude = 2 * args_in.body_size;

    {
        RigidBody::Args args;
        args.initial_pose = { x, y, theta };
        args.mass = density * (4.0/3) * M_PI * pow(args_in.body_size/2, 3);
        args.moment_of_inertia = args.mass * 0.4 * (args_in.body_size/2);
        agent.body_rigid_body = create_rigid_body(args);
    }
    const RigidBody& body_rigid_body = rigid_bodies[agent.body_rigid_body];

    {
        Circle::Args args;
        args.color = glm::vec4(0, 0.6, 0, 1);
        args.radius = args_in.body_size / 2;
        args.edge_width = 0;
        agent.body_circle = create_circle(args);
    }

    {
        Hand::Args args;
        args.initial_pose = { 0, -agent.hand_separation / 2, 0 };
        args.size = args_in.hand_size;
        agent.hand_right = create_hand(args);
    }
    {
        Hand::Args args;
        args.initial_pose = { 0, agent.hand_separation / 2, 0 };
        args.size = args_in.hand_size;
        agent.hand_left = create_hand(args);
    }

    return index;
}

void State::remove_agent(int index)
{
    const Agent& agent = agents[index];

    remove_rigid_body(agent.body_rigid_body);
    remove_circle(agent.body_circle);
    remove_hand(agent.hand_right);
    remove_hand(agent.hand_left);

    agents.remove(index);
}

void State::update_agents()
{
    for (int i = 0; i < agents.size(); i++) {
        Agent& agent = agents[i];
        if (!agent.valid) continue;

        RigidBody& rigid_body = rigid_bodies[agent.body_rigid_body];

        Pose pose_error(0, 0, 0);
        if (window.have_world_view) {
            pose_error.pos() = window.mouse_pos - rigid_body.pose.pos();
            pose_error.theta() = clamp_angle(std::atan2(pose_error.pos().y, pose_error.pos().x) - rigid_body.pose.theta());
        }
        if (glm::length(pose_error.pos()) < 0.01) {
            agent.twist_target = glm::zero<glm::vec3>();
        } else {
            agent.twist_target = agent.follow_gain * pose_error;
            float lin_speed = glm::length(agent.twist_target.lin());
            float ang_speed = std::fabs(agent.twist_target.ang());
            if (lin_speed > 20.0) {
                agent.twist_target.lin() *= (20.0f / lin_speed);
            }
            if (ang_speed > 20.0) {
                agent.twist_target.ang() *= (20.0f / ang_speed);
            }
        }

        rigid_body.wrench = agent.twist_gain * (agent.twist_target - rigid_body.twist);

        Circle& circle = circles[agent.body_circle];
        circle.pos = rigid_body.pose.pos();

        agent.hand_swing_phase += clock.dt * agent.hand_swing_freq * (2 * M_PI);

        float speed = glm::length(rigid_body.twist.lin());
        float decayed_amplitude = (1 - std::exp(-speed / 10.0)) * agent.hand_swing_amplitude;

        Hand& hand_right = hands[agent.hand_right];
        hand_right.pose_target = rigid_body.pose * Pose(
            decayed_amplitude * std::sin(agent.hand_swing_phase),
            -agent.hand_separation / 2,
            0
        );

        Hand& hand_left = hands[agent.hand_left];
        hand_left.pose_target = rigid_body.pose * Pose(
            decayed_amplitude * std::sin(agent.hand_swing_phase + M_PI),
            agent.hand_separation / 2,
            0
        );
    }
}

int State::create_hand(Hand::Args args_in)
{
    int index = hands.create();
    Hand& hand = hands[index];
    hand.pose_target = glm::zero<glm::vec3>();
    hand.gain = glm::diagonal3x3(glm::vec3(0.3, 0.3, 0.001));
    hand.damping = glm::diagonal3x3(glm::vec3(0.01, 0.01, 0.01));

    {
        RigidBody::Args args;
        args.initial_pose = args_in.initial_pose;
        args.mass = density * (4.0/3) * M_PI * pow(args_in.size/2, 3);
        args.moment_of_inertia = args.mass * 0.4 * (args_in.size/2);
        hand.rigid_body = create_rigid_body(args);
    }

    {
        Circle::Args args;
        args.color = glm::vec4(0, 0, 0, 1);
        args.radius = args_in.size / 2;
        args.edge_width = 0;
        hand.circle = create_circle(args);
    }

    return index;
}

void State::remove_hand(int index)
{
    remove_rigid_body(index);
    remove_circle(index);

    hands.remove(index);
}

void State::update_hands()
{
    for (int i = 0; i < hands.size(); i++) {
        Hand& hand = hands[i];
        if (!hand.valid) continue;

        RigidBody& rigid_body = rigid_bodies[hand.rigid_body];
        rigid_body.wrench = hand.gain * (hand.pose_target - rigid_body.pose) - hand.damping * rigid_body.twist;

        Circle& circle = circles[hand.circle];
        circle.pos = rigid_body.pose;
    }
}

int State::create_rigid_body(RigidBody::Args args_in)
{
    int index = rigid_bodies.create();
    RigidBody& rigid_body = rigid_bodies[index];

    rigid_body.pose = args_in.initial_pose;
    rigid_body.twist = { 0, 0, 0 };
    rigid_body.wrench = { 0, 0, 0 };
    rigid_body.inv_mass_matrix = {
        1.0 / args_in.mass, 0, 0,
        0, 1.0 / args_in.mass, 0,
        0, 0, 1.0 / args_in.moment_of_inertia
    };

    return index;
}

void State::remove_rigid_body(int index)
{
    rigid_bodies.remove(index);
}

void State::update_rigid_bodies()
{
    for (int i = 0; i < rigid_bodies.size(); i++) {
        RigidBody& rigid_body = rigid_bodies[i];
        if (!rigid_body.valid) continue;

        Spatial acc = rigid_body.inv_mass_matrix * rigid_body.wrench;
        float dt = std::min(clock.dt, 0.1f);
        rigid_body.pose += rigid_body.twist * dt + acc * 0.5f * std::pow(dt, 2.0f);
        rigid_body.twist += acc * dt;
    }
}
