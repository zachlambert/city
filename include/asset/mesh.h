#pragma once

#include <glm/glm.hpp>
#include <vector>


struct MeshVertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 color;
};

void append_box(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    const glm::vec3& size);

void append_cylinder(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float length,
    float resolution);

void append_sphere(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float resolution);

void append_capsule(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float length,
    float resolution);

void append_plane(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    const glm::vec3& normal,
    const glm::vec3& dir_depth,
    float depth,
    float width,
    const glm::vec3& offset = glm::vec3(0, 0, 0));

void append_circle(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    const glm::vec3& normal,
    float radius,
    float resolution,
    const glm::vec3& offset = glm::vec3(0, 0, 0));

void translate_vertices(std::vector<MeshVertex>& vertices, const glm::vec3& pos);
