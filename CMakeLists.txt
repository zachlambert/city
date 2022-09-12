cmake_minimum_required(VERSION 3.0)
project(
    "City game"
    VERSION 0.0.1
    DESCRIPTION "City game."
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 20)

find_package(yaml-cpp REQUIRED NO_MODULE)

add_executable(game
    src/asset/mesh.cpp
    src/asset/shader.cpp
    src/component/agent.cpp
    src/component/mesh.cpp
    src/component/rigid_body.cpp
    src/maths/collision.cpp
    src/system/agent_handler.cpp
    src/system/camera_handler.cpp
    src/system/clock_handler.cpp
    src/system/mesh_renderer.cpp
    src/system/physics.cpp
    src/system/terrain_renderer.cpp
    src/system/window_handler.cpp
    # src/world/builder.cpp
    src/main.cpp
)

target_include_directories(game
    PRIVATE include
)

target_link_libraries(game PRIVATE GL GLU glfw GLEW yaml-cpp)
