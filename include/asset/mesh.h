#pragma once

#include <glm/glm.hpp>
#include <vector>


struct MeshVertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 color;
};

void generate_box(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    const glm::vec3& size);

void generate_cylinder(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float length,
    float resolution);

void generate_sphere(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float resolution);

void generate_capsule(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float length,
    float resolution);
