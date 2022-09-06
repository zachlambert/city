#pragma once

#include "component/component.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

namespace glm {

inline glm::vec2 rotate(float rotation, const glm::vec2& x)
{
    const glm::mat2 R = { std::cos(rotation), std::sin(rotation), -std::sin(rotation), std::cos(rotation) };
    return R * x;
}

} // namespace glm

struct Pose: public glm::vec3 {
    glm::vec2& pos() { return *(glm::vec2*)(this); }
    const glm::vec2& pos()const { return *(glm::vec2*)(this); }
    float& theta() { return z; } 
    float theta()const { return z; } 
    Pose() {}
    Pose(const glm::vec3& vec) { *(glm::vec3*)(this) = vec; }
    Pose(const glm::vec2& pos_, float theta_) { pos() = pos_; theta() = theta_; }
    Pose(float pos_x, float pos_y, float theta_) { x = pos_x; y = pos_y; theta() = theta_; }
};

struct Spatial: public glm::vec3 {
    glm::vec2& lin() { return *(glm::vec2*)(this); }
    const glm::vec2& lin()const { return *(glm::vec2*)(this); }
    float& ang() { return z; } 
    float ang()const { return z; } 
    Spatial() {}
    Spatial(const glm::vec3& vec) { *(glm::vec3*)(this) = vec; }
    Spatial(const glm::vec2& lin_, float ang_) { lin() = lin_; ang() = ang_; }
    Spatial(float lin_x, float lin_y, float ang_) { x = lin_x; y = lin_y; ang() = ang_; }
};

inline Pose operator*(const Pose& lhs, const Pose& rhs)
{
    Pose result;
    result.pos() = lhs.pos() + glm::rotate(lhs.theta(), rhs.pos());
    result.theta() = lhs.theta() + rhs.theta();
    return result;
}

inline float clamp_angle(float angle)
{
    while (angle > M_PI) angle -= 2 * M_PI;
    while (angle < -M_PI) angle += 2 * M_PI;
    return angle;
}

struct RigidBody: public Component {
    struct Args {
        Pose initial_pose;
        float mass;
        float moment_of_inertia;
    };
    Pose pose;
    Spatial twist;
    Spatial wrench;
    glm::mat3 inv_mass_matrix;
};

struct Hand: public Component {
    struct Args {
        float size;
        Pose initial_pose;
        float gain_lin;
        float gain_ang;
        float damping_lin;
        float damping_ang;
    };
    Pose pose_target;
    glm::mat3 gain;
    glm::mat3 damping;

    int rigid_body;
    int circle;
};

struct Agent: public Component {
    struct Args {
        float body_size;
        float hand_size;
    };

    Spatial twist_target;
    glm::mat3 follow_gain;
    glm::mat3 twist_gain;
    float hand_separation;
    float hand_swing_freq;
    float hand_swing_phase;
    float hand_swing_amplitude;

    int body_rigid_body;
    int body_circle;
    int hand_left;
    int hand_right;
};
