
#include "component/mesh.h"


int create_mesh(
    MeshList& meshes,
    const Mesh::Args& args
){
    int index = meshes.create();
    Mesh& mesh = meshes[index];

    mesh.mesh_id = args.mesh_id;
    mesh.pose = Pose::identity();
    mesh.scale = args.scale;

    return index;
}

void destroy_mesh(
    MeshList& meshs,
    int index)
{
    meshs.remove(index);
}
