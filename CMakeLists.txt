cmake_minimum_required(VERSION 3.0)
project(
    "City game"
    VERSION 0.0.1
    DESCRIPTION "City game."
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 20)

add_executable(game
    src/main.cpp
    src/shader.cpp
    src/component/agent.cpp
    src/component/camera.cpp
    src/component/clock.cpp
    src/component/game.cpp
    src/component/shape.cpp
    src/component/shape_renderer.cpp
    src/component/window.cpp
)

target_include_directories(game
    PRIVATE include
)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE /opt/stb)

target_link_libraries(game PRIVATE GL GLU glfw GLEW assimp stb tinyxml)
