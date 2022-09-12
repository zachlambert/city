
#include "maths/collision.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


CapsuleCollision capsule_collision(const Capsule& a, const Capsule& b)
{
    const glm::vec3 ua = normalize(a.end - a.start);
    const glm::vec3 ub = normalize(b.end - b.start);
    const glm::vec3 norm = glm::cross(ua, ub);
    const glm::vec3 va = glm::cross(norm, ua);
    const glm::vec3 vb = glm::cross(norm, ub);

    const glm::mat3 Ra = glm::transpose(glm::mat3(ua, va, norm));
    const glm::mat3 Rb = glm::mat3(ub, vb, norm);
    auto project_on_a = [&](const glm::vec3& x) {
        return glm::transpose(Ra) * (x - a.start);
    };
    auto project_on_b = [&](const glm::vec3& x) {
        return glm::transpose(Ra) * (x - a.start);
    };

    const float a_length = glm::length(a.end - a.start);
    const float b_length = glm::length(b.end - b.start);
    const glm::vec3 a0_b = project_on_b(a.start);
    const glm::vec3 a1_b = project_on_b(a.end);
    const glm::vec3 b0_a = project_on_a(b.start);
    const glm::vec3 b1_a = project_on_a(b.end);

    // A line crosses the other if the sign of the y components differ
    bool a_crosses = a0_b.y * a1_b.y < 0;
    bool b_crosses = b0_a.y * b1_a.y < 0;

    CapsuleCollision result;

    if (a_crosses && b_crosses) {
        // Intersection
        float a_location = std::fabs(a0_b.y) / (std::fabs(a0_b.y) + std::fabs(a1_b.y));
        result.closest_a = a.start + a_location * a_length;
        float b_location = std::fabs(b0_a.y) / (std::fabs(b0_a.y) + std::fabs(b1_a.y));
        result.closest_b = b.start + b_location * b_length;

    } else if (a_crosses) {
        // b lies completely to one side, whichever endpoint is closer
        // is the closest point
        if (std::fabs(b0_a.y) < std::fabs(b1_a.y)) {
            // b.start is closer, at a distance (b0_a.x) along ua
            result.closest_b = b.start;
            result.closest_a = a.start + b0_a.x * ua;
        } else {
            // b.end is closer, at a distance (b1_a.x) along ua
            result.closest_b = b.end;
            result.closest_a = a.start + b1_a.x * ua;
        }
    } else if (b_crosses) {
        // a lies completely to one side, whichever endpoint is closer
        // is the closest point
        if (std::fabs(a0_b.y) < std::fabs(a1_b.y)) {
            // a.start is closer, at a distance (a0_b.x) along ub
            result.closest_a = a.start;
            result.closest_b = b.start + a0_b.x * ub;
        } else {
            // a.end is closer, at a distance (a1_b.x) along ub
            result.closest_a = a.end;
            result.closest_b = b.start + a1_b.x * ub;
        }
    } else {
        if (std::fabs(b0_a.y) < std::fabs(b1_a.y)) {
            result.closest_b = b.start;
        } else {
            result.closest_b = b.end;
        }
        if (std::fabs(a0_b.y) < std::fabs(a1_b.y)) {
            result.closest_a = a.start;
        } else {
            result.closest_a = a.end;
        }
    }

    result.distance = glm::length(result.closest_b - result.closest_a) - (a.radius + b.radius);
    if (result.distance < 0) {
        result.collision = true;
    }

    return result;
}
