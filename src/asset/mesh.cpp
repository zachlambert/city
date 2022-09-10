
#include "asset/mesh.h"


void append_box(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    const glm::vec3& size)
{
    MeshVertex vertex;
    vertex.color = color;
    for (size_t i = 0; i < 6; i++) {
        float dir = (i < 3 ? -1 : 1);
        size_t out = i % 3;
        size_t right = (out+1)%3;
        size_t up = (right+1)%3;

        vertex.normal[out] = dir;
        vertex.normal[right] = 0;
        vertex.normal[up] = 0;
        vertex.pos[out] = dir * size[out] / 2;

        size_t offset = vertices.size();
        for (size_t j = 0; j < 4; j++) {
            vertex.pos[right] = (j % 2 == 0 ? -1 : 1) * size[right] / 2;
            vertex.pos[up] = (j < 2 ? -1 : 1) * size[up] / 2;
            vertices.push_back(vertex);
        }

        if (i < 3) {
            indices.push_back(offset + 2);
            indices.push_back(offset + 1);
            indices.push_back(offset + 0);
            indices.push_back(offset + 2);
            indices.push_back(offset + 3);
            indices.push_back(offset + 1);
        } else {
            indices.push_back(offset + 0);
            indices.push_back(offset + 1);
            indices.push_back(offset + 2);
            indices.push_back(offset + 1);
            indices.push_back(offset + 3);
            indices.push_back(offset + 2);
        }
    }
}

void append_cylinder(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float length,
    float resolution)
{
    MeshVertex vertex;
    vertex.color = color;

    const size_t N = ceil(2*radius*M_PI / resolution);

    // Top face

    size_t offset = 0;
    vertex.pos = glm::vec3(0, 0, length);
    vertex.normal = glm::vec3(0, 0, 1);
    vertices.push_back(vertex);
    for (size_t i = 0; i < N; i++) {
        float theta = i*2*M_PI / N;
        vertex.pos = glm::vec3(radius * cos(theta), radius * sin(theta), length);
        vertices.push_back(vertex);
    }
    for (size_t i = 0; i < N; i++) {
        indices.push_back(offset);
        indices.push_back(offset + 1 + i);
        indices.push_back(offset + 1 + (i+1)%N);
    }

    // Curved surface

    offset = vertices.size();
    for (size_t i = 0; i < N; i++) {
        double theta = i*2*M_PI / N;
        vertex.pos = glm::vec3(radius * cos(theta), radius * sin(theta), length);
        vertex.normal = glm::vec3(cos(theta), sin(theta), 0);
        vertices.push_back(vertex);
        vertex.pos.z = 0;
        vertices.push_back(vertex);
    }
    for (size_t i = 0; i < 2*N; i+=2) {
        indices.push_back(offset + i);
        indices.push_back(offset + i+1);
        indices.push_back(offset + (i+2)%(2*N));
        indices.push_back(offset + i+1);
        indices.push_back(offset + (i+3)%(2*N));
        indices.push_back(offset + (i+2)%(2*N));
    }

    // Bottom face

    offset = vertices.size();
    vertex.pos = glm::vec3(0, 0, 0);
    vertex.normal = glm::vec3(0, 0, -1);
    vertices.push_back(vertex);
    for (size_t i = 0; i < N; i++) {
        double theta = i*2*M_PI / N;
        vertex.pos = glm::vec3(radius * cos(-theta), radius * sin(-theta), 0);
        vertices.push_back(vertex);
    }
    for (size_t i = 0; i < N; i++) {
        indices.push_back(offset);
        indices.push_back(offset + 1 + i);
        indices.push_back(offset + 1 + (i+1)%N);
    }
}

void append_sphere(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float resolution)
{
    // TODO: Replace with mesh of a icosahedron instead

    const size_t N = ceil(radius*M_PI / resolution);
    float phi, theta;

    MeshVertex vertex;
    vertex.color = color;
    vertex.pos = glm::vec3(0, 0, radius);
    vertex.normal = glm::vec3(0, 0, 1);
    vertices.push_back(vertex);
    for (size_t i = 1; i < N; i++) {
        phi = (M_PI*(float)i)/N;
        vertex.pos.z = radius * cos(phi);
        for (size_t j = 0; j < 2*N; j++) {
            theta = (2*M_PI*(float)j)/(2*N); 
            vertex.pos.x = radius * sin(phi) * cos(theta);
            vertex.pos.y = radius * sin(phi) * sin(theta);
            vertex.normal = glm::vec3(sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi));
            vertices.push_back(vertex);
        }
    }
    vertex.pos = glm::vec3(0, 0, -radius);
    vertex.normal = glm::vec3(0, 0, 1);
    vertices.push_back(vertex);

    // Append to indices

    // Top strip, common vertex = top
    for (size_t j = 0; j < 2*N; j++) {
        indices.push_back(0);
        indices.push_back(1+j);
        if (j < 2*N-1) {
            indices.push_back(2+j);
        } else {
            indices.push_back(1);
        }
    }
    // Bottom strip, common vertex = bottom
    size_t bot = vertices.size() - 1;
    for (size_t j = 0; j < 2*N; j++) {
        indices.push_back(bot);
        indices.push_back(bot-1-j);
        if (j < 2*N-1)
            indices.push_back(bot-2-j);
        else
            indices.push_back(bot-1);
    }
    // Remaining strips, made up of rectangles between
    size_t strip_1_start, strip_2_start;
    for (size_t i = 0; i < N-2; i++) {
        strip_1_start = 1 + 2*N*i;
        strip_2_start = 1 + 2*N*(i+1);
        for (size_t j = 0; j < 2*N; j++) {
            // First triangle of rectangle
            indices.push_back(strip_1_start + j);
            indices.push_back(strip_2_start + j);
            if (j < 2*N-1) {
                indices.push_back(strip_2_start + j + 1);
            } else {
                indices.push_back(strip_2_start);
            }
            // Second triangle of rectangle
            if (j < 2*N-1) { 
                indices.push_back(strip_2_start + j + 1);
                indices.push_back(strip_1_start + j + 1);
            } else {
                indices.push_back(strip_2_start);
                indices.push_back(strip_1_start);
            }
            indices.push_back(strip_1_start + j);
        }
    }
}

void append_capsule(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    float radius,
    float length,
    float resolution)
{
    size_t N = ceil(radius*M_PI / resolution);
    N += (N % 2); // Must be even
    float phi, theta;

    float top_centre = length / 2;
    float bot_centre = -length / 2;

    MeshVertex vertex;
    vertex.color = color;
    vertex.pos = glm::vec3(0, 0, top_centre + radius);
    vertex.normal = glm::vec3(0, 0, 1);
    vertices.push_back(vertex);
    for (size_t i = 1; i < N + 1; i++) {
        if (i <= N/2) {
            phi = (M_PI*(float)i)/N;
            vertex.pos.z = top_centre + radius * cos(phi);
        } else {
            phi = (M_PI*(float)(i-1))/N;
            vertex.pos.z = bot_centre + radius * cos(phi);
        }
        for (size_t j = 0; j < 2*N; j++) {
            theta = (2*M_PI*(float)j)/(2*N); 
            vertex.pos.x = radius * sin(phi) * cos(theta);
            vertex.pos.y = radius * sin(phi) * sin(theta);
            vertex.normal = glm::vec3(sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi));
            vertices.push_back(vertex);
        }
    }
    vertex.pos = glm::vec3(0, 0, bot_centre);
    vertex.normal = glm::vec3(0, 0, -1);
    vertices.push_back(vertex);

    // Append to indices

    // Top strip, common vertex = top
    for (size_t j = 0; j < 2*N; j++) {
        indices.push_back(0);
        indices.push_back(1+j);
        if (j < 2*N-1)
            indices.push_back(2+j);
        else
            indices.push_back(1);
    }
    // Bottom strip, common vertex = bottom
    size_t bot = vertices.size() - 1;
    for (size_t j = 0; j < 2*N; j++) {
        indices.push_back(bot);
        indices.push_back(bot-1-j);
        if (j < 2*N-1)
            indices.push_back(bot-2-j);
        else
            indices.push_back(bot-1);
    }
    // Remaining strips, made up of rectangles between
    size_t strip_1_start, strip_2_start;
    for (size_t i = 0; i < N-1; i++) {
        strip_1_start = 1 + 2*N*i;
        strip_2_start = 1 + 2*N*(i+1);
        for (size_t j = 0; j < 2*N; j++) {
            // First triangle of rectangle
            indices.push_back(strip_1_start + j);
            indices.push_back(strip_2_start + j);
            if (j < 2*N-1) {
                indices.push_back(strip_2_start + j + 1);
            } else {
                indices.push_back(strip_2_start);
            }
            // Second triangle of rectangle
            if (j < 2*N-1) { 
                indices.push_back(strip_2_start + j + 1);
                indices.push_back(strip_1_start + j + 1);
            } else {
                indices.push_back(strip_2_start);
                indices.push_back(strip_1_start);
            }
            indices.push_back(strip_1_start + j);
        }
    }
}

void append_plane(
    std::vector<MeshVertex>& vertices,
    std::vector<unsigned short>& indices,
    const glm::vec4& color,
    const glm::vec3& normal,
    const glm::vec3& dir_depth,
    float depth,
    float width)
{
    MeshVertex vertex;
    vertex.color = color;
    vertex.normal = normal;

    glm::vec3 u1 = dir_depth;
    glm::vec3 u2 = glm::cross(normal, dir_depth);

    for (size_t i = 0; i < 4; i++) {
        vertex.pos =
            (i % 2 == 0 ? -1 : 1) * 0.5f * depth * u1
            + (i < 2 ? -1 : 1) * 0.5f * width * u2;
        vertices.push_back(vertex);
    }

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(2);
}

void translate_vertices(std::vector<MeshVertex>& vertices, const glm::vec3& pos)
{
    for (auto& vertex: vertices) {
        vertex.pos += pos;
    }
}
