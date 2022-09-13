#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>


struct Pose {
    glm::vec3 pos;
    glm::mat3 orient;

    Pose inverse()const;
    glm::mat4 to_mat4()const;
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

Pose operator*(const Pose& lhs, const Pose& rhs);
void operator+=(Spatial& lhs, const Spatial& rhs);
void operator-=(Spatial& lhs, const Spatial& rhs);

Spatial operator+(Spatial lhs, const Spatial& rhs);
Spatial operator-(Spatial lhs, const Spatial& rhs);
Spatial operator-(Spatial spatial);
void operator*=(Spatial& spatial, float scalar);
Spatial operator*(Spatial spatial, float scalar);
Spatial operator*(float scalar, Spatial spatial);

Spatial operator*(const SpatialInertia& inertia, const Spatial& spatial);

Spatial adjoint_map_twist(const Pose& pose, const Spatial& twist);

Spatial adjoint_map_momentum(const Spatial& twist, const Spatial& momentum);

Pose matrix_exp(const Spatial& twist);

// Transforms from X = Forward, Y = Left, Z = Up
// to opengl frame, -Z = Forward, -X = Left, Y = Up
// Need to use when calculating projection matrix, and to work with some
// of the glm functions that use the opengl coordinate system
static constexpr glm::mat3 coord_system_fix()
{
    return glm::mat3(
        glm::vec3(0, 0, -1),
        glm::vec3(-1, 0, 0),
        glm::vec3(0, 1, 0)
    );
}

glm::mat3 euler_to_rotation(const glm::vec3& euler);
