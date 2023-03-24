cmake_minimum_required(VERSION 3.24)
project(Engine_Tutorial)
set(CMAKE_BUILD_TYPE Debug)
add_definitions(-w)

set(DynamicLink OFF)
set(gladPath ${CMAKE_SOURCE_DIR}/Engine/Include/glad)
set(GLFWPath ${CMAKE_SOURCE_DIR}/Engine/Include/GLFW)
set(ImGuiPath ${CMAKE_SOURCE_DIR}/Engine/Include/imgui)

message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("gladPath: ${gladPath}")
message("GLFWPath: ${GLFWPath}")
message("ImGuiPath: ${ImGuiPath}")

set(CMAKE_CXX_STANDARD 17)

message(${CMAKE_CXX_COMPILER_ID})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin-C)
link_directories(${CMAKE_SOURCE_DIR}/Lib)

add_compile_definitions(Z_PLATFORM_WIN32 Z_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
include_directories(${CMAKE_SOURCE_DIR}/Engine/Include/spdlog/include
        ${CMAKE_SOURCE_DIR}/Engine/ ${CMAKE_SOURCE_DIR}/Engine/Include/glm
        ${CMAKE_SOURCE_DIR}/Engine/Include
        ${ImGuiPath}
        depends)
add_subdirectory(Engine/Include/GLFW)
add_subdirectory(Engine/Include/glad)
add_subdirectory(Engine/Include/imgui)

add_subdirectory(Engine)

add_dependencies(Z glad GLFW ImGui)
#add_subdirectory(Test)

add_subdirectory(ZEditor)