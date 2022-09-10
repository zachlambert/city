
#include "system/agent_handler.h"
#include "system/camera_handler.h"
#include "system/clock_handler.h"
#include "system/mesh_renderer.h"
#include "system/terrain_renderer.h"
#include "system/physics.h"
#include "system/window_handler.h"
#include "world/builder.h"
#include <yaml-cpp/yaml.h>


int main()
{
    auto config = YAML::LoadFile("config.yaml");

    Camera camera;
    Clock clock;
    Input input;
    Viewport viewport;
    World world;

    MeshList meshes;
    RigidBodyList rigid_bodies;
    AgentList agents(meshes, rigid_bodies);
    
    WindowHandler window_handler(viewport, input, camera, [&config]() {
        WindowHandler::Args args;
        auto window = config["window"];
        args.width = window["width"].as<size_t>();
        args.height = window["height"].as<size_t>();
        args.bg = glm::vec4(0.6, 0.6, 0.6, 1);
        args.fullscreen = false;
        args.name = "City";
        args.keys = {
            GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_S, GLFW_KEY_W,
            GLFW_KEY_LEFT_SHIFT, GLFW_KEY_SPACE,
            GLFW_KEY_Q, GLFW_KEY_E,
            GLFW_KEY_1, GLFW_KEY_2
        };
        return args;
    }());

    MeshRenderer mesh_renderer(viewport, meshes, [&config]() {
        MeshRenderer::Args args;
        args.shader_path = config["shader_path"].as<std::string>();
        return args;
    }());

    TerrainRenderer terrain_renderer(viewport, world, [&config]() {
        TerrainRenderer::Args args;
        args.shader_path = config["shader_path"].as<std::string>();
        return args;
    }());

    ClockHandler clock_handler(clock);

    CameraHandler camera_handler(camera, input, clock, [&config]() {
        CameraHandler::Args args;
        return args;
    }());

    Physics physics(clock, rigid_bodies, [&config]() {
        return Physics::Args();
    }());

    AgentHandler agent_handler(agents, clock, mesh_renderer, [&config]() {
        return AgentHandler::Args();
    }());

    {
        Builder builder(world, terrain_renderer);
        builder.start();
        builder.finish();
    }

    while (viewport.open) {
        window_handler.tick();
        clock_handler.tick();
        camera_handler.tick();
        agent_handler.tick();
        physics.tick();
        mesh_renderer.tick();
        terrain_renderer.tick();
    }
    return 0;
}
