
#include "maths/geometry.h"


Pose Pose::inverse()const
{
    Pose result;
    result.orient = glm::transpose(orient);
    result.pos = -result.orient * pos;
    return result;
}
glm::mat4 Pose::to_mat4()const
{
    // https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.20.pdf
    // Section 5.4.2 for converting mat3 -> mat4
    return glm::translate(pos) * glm::mat4x4(orient);
}

Pose operator*(const Pose& lhs, const Pose& rhs)
{
    Pose result;
    result.pos = lhs.pos + lhs.orient * rhs.pos;
    result.orient = lhs.orient * rhs.orient;
    return result;
}
void operator+=(Spatial& lhs, const Spatial& rhs)
{
    lhs.lin += rhs.lin;
    lhs.ang += rhs.ang;
}
void operator-=(Spatial& lhs, const Spatial& rhs)
{
    lhs.lin -= rhs.lin;
    lhs.ang -= rhs.ang;
}

Spatial operator+(Spatial lhs, const Spatial& rhs)
{
    lhs += rhs;
    return lhs;
}
Spatial operator-(Spatial lhs, const Spatial& rhs)
{
    lhs -= rhs;
    return lhs;
}
Spatial operator-(Spatial spatial)
{
    spatial.lin = -spatial.lin;
    spatial.ang = -spatial.ang;
    return spatial;
}
void operator*=(Spatial& spatial, float scalar)
{
    spatial.lin *= scalar;
    spatial.ang *= scalar;
}
Spatial operator*(Spatial spatial, float scalar)
{
    spatial *= scalar;
    return spatial;
}
Spatial operator*(float scalar, Spatial spatial)
{
    return spatial * scalar;
}

Spatial operator*(const SpatialInertia& inertia, const Spatial& spatial)
{
    Spatial result;
    result.ang = inertia.inertia * spatial.ang;
    result.lin = inertia.mass * spatial.lin;
    return result;
}

Spatial adjoint_map_twist(const Pose& pose, const Spatial& twist)
{
    Spatial result;
    result.ang = pose.orient * twist.ang;
    result.lin = glm::cross(pose.pos, pose.orient * twist.ang) + twist.lin;
    return result;
}

Spatial adjoint_map_momentum(const Spatial& twist, const Spatial& momentum)
{
    Spatial result;
    result.ang = glm::cross(twist.lin, momentum.ang) - glm::cross(twist.ang, momentum.ang);
    result.lin = glm::cross(momentum.lin, twist.ang);
    return result;
}

Pose matrix_exp(const Spatial& twist)
{
    Pose pose;
    pose.pos = glm::zero<glm::vec3>();

    float angle = glm::length(twist.ang);
    if (angle < 1e-12) {
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

glm::mat3 euler_to_rotation(const glm::vec3& euler)
{
    glm::mat3 result;
    result = glm::orientate3(glm::vec3(-euler.z, euler.y, euler.x));
    result =
        coord_system_fix()
        * result
        * glm::transpose(coord_system_fix());
    return result;
}
