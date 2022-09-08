#pragma once

#include "component/component.h"
#include <glm/glm.hpp>
#include "maths/geometry.h"

struct Mesh {
    struct Args {
        float scale;
        int mesh_id;
    };

    Pose pose;
    float scale;
    int mesh_id;
    bool valid;
};
typedef ComponentList<Mesh> MeshList;

int create_mesh(
    MeshList& meshes,
    const Mesh::Args& args
);

void destroy_mesh(
    MeshList& meshes,
    int index
);
