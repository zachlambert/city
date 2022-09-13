#pragma once

#include <glm/glm.hpp>


struct Capsule {
    glm::vec3 start;
    glm::vec3 end;
    float radius;
    Capsule():
        start(0, 0, 0),
        end(0, 0, 0),
        radius(0)
    {}
    Capsule(const glm::vec3& start, const glm::vec3& end, float radius=0):
        start(start), end(end), radius(radius)
    {}
};
struct CapsuleCollision {
    bool collision;
    float distance;
    // Points on the capsule line that are closest
    glm::vec3 closest_a;
    glm::vec3 closest_b;
};

CapsuleCollision capsule_collision(const Capsule& a, const Capsule& b);
