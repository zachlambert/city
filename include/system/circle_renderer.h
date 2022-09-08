#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <string>

#include "system/window.h"
#include "component/circle.h"


class CircleRenderer {
public:
    struct Args {
        std::string shader_path;
    };
    CircleRenderer(
        const WindowState& window_state,
        const ComponentList<Circle>& circles,
        const Args& args);
    void tick();

private:
    const WindowState& window_state;
    const CircleList& circles;

    unsigned int VAO;
    unsigned int quad_VBO;
    unsigned int instance_VBO;

    unsigned int program_id;
    unsigned int v_loc;

    std::array<glm::vec2, 6> quad_vertices;
};
