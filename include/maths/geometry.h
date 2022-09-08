#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


struct Pose {
    glm::vec3 pos;
    glm::mat3 orient;

    Pose inverse()const
    {
        Pose result;
        result.orient = glm::transpose(orient);
        result.pos = -orient * pos;
        return result;
    }
    glm::mat4 to_mat4()const
    {
        // https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.20.pdf
        // Section 5.4.2 for converting mat3 -> mat4
        return glm::translate(pos) * glm::mat4x4(orient);
    }
    static constexpr Pose identity() {
        return { glm::zero<glm::vec3>(), glm::identity<glm::mat3>() };
    }
};

struct Spatial {
    glm::vec3 ang;
    glm::vec3 lin;
    static constexpr Spatial zero() {
        return { glm::zero<glm::vec3>(), glm::zero<glm::vec3>() };
    };
};

struct SpatialInertia {
    glm::mat3 inertia;
    float mass;
};

inline Pose operator*(const Pose& lhs, const Pose& rhs)
{
    Pose result;
    result.pos = lhs.pos + lhs.orient * rhs.pos;
    result.orient = lhs.orient * rhs.orient;
    return result;
}
inline void operator+=(Spatial& lhs, const Spatial& rhs)
{
    lhs.lin += rhs.lin;
    lhs.ang += rhs.ang;
}
inline void operator-=(Spatial& lhs, const Spatial& rhs)
{
    lhs.lin -= rhs.lin;
    lhs.ang -= rhs.ang;
}

inline Spatial operator+(Spatial lhs, const Spatial& rhs)
{
    lhs += rhs;
    return lhs;
}
inline Spatial operator-(Spatial lhs, const Spatial& rhs)
{
    lhs -= rhs;
    return lhs;
}
inline Spatial operator-(Spatial spatial)
{
    spatial.lin = -spatial.lin;
    spatial.ang = -spatial.ang;
    return spatial;
}
inline void operator*=(Spatial& spatial, float scalar)
{
    spatial.lin *= scalar;
    spatial.ang *= scalar;
}
inline Spatial operator*(Spatial spatial, float scalar)
{
    spatial *= scalar;
    return spatial;
}
inline Spatial operator*(float scalar, Spatial spatial)
{
    return spatial * scalar;
}

inline Spatial operator*(const SpatialInertia& inertia, const Spatial& spatial)
{
    Spatial result;
    result.ang = inertia.inertia * spatial.ang;
    result.lin = inertia.mass * spatial.lin;
    return result;
}

inline Spatial adjoint_map_twist(const Pose& pose, const Spatial& twist)
{
    Spatial result;
    result.ang = pose.orient * twist.ang;
    result.lin = glm::cross(pose.pos, pose.orient * twist.ang) + twist.lin;
    return result;
}

inline Spatial adjoint_map_momentum(const Spatial& twist, const Spatial& momentum)
{
    Spatial result;
    result.ang = glm::cross(twist.lin, momentum.ang) - glm::cross(twist.ang, momentum.ang);
    result.lin = glm::cross(momentum.lin, twist.ang);
    return result;
}

inline Pose matrix_exp(const Spatial& twist)
{
    Pose pose;
    pose.pos = glm::zero<glm::vec3>();

    float angle = glm::length(twist.ang);
    if (angle > 1e-12) {
        pose.orient = glm::identity<glm::mat3>();
        return pose;
    }

    glm::vec3 axis = twist.ang / angle;
    pose.orient = glm::toMat3(glm::angleAxis(angle, axis));

    float length = glm::length(twist.lin);
    if (length > 1e-12) {
        glm::vec3 lin_par = (twist.lin / length) * glm::dot(twist.lin, axis);
        glm::vec3 lin_perp_hat = (twist.lin - lin_par) / angle;
        pose.pos = lin_par + lin_perp_hat*std::sin(angle) + glm::cross(axis, lin_perp_hat)*(1 - std::cos(angle));
    }

    return pose;
}

inline float clamp_angle(float angle)
{
    if (angle < 0) angle += 2 * M_PI;
    angle = std::fmod(angle, 2 * M_PI);
    if (angle > M_PI) angle -= 2 * M_PI;
    return angle;
}
