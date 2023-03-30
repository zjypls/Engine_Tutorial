cmake_minimum_required(VERSION 3.24)
project(Engine_Tutorial)
set(CMAKE_BUILD_TYPE Debug)
add_definitions(-w)

set(DynamicLink OFF)
set(gladPath ${CMAKE_SOURCE_DIR}/Engine/Include/glad)
set(GLFWPath ${CMAKE_SOURCE_DIR}/Engine/Include/GLFW)
set(ImGuiPath ${CMAKE_SOURCE_DIR}/Engine/Include/imgui)
set(enttPath ${CMAKE_SOURCE_DIR}/Engine/Include/entt/include)
set(yamlPath ${CMAKE_SOURCE_DIR}/Engine/Include/yaml-cpp/include)
set(ImGuizmoPath ${CMAKE_SOURCE_DIR}/Engine/Include/ImGuizmo)

message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")


set(CMAKE_CXX_STANDARD 17)

message(${CMAKE_CXX_COMPILER_ID})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib)
if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ZEditor)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/Bin-C/${CMAKE_CXX_COMPILER_ID}>)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin-C/${CMAKE_CXX_COMPILER_ID})
endif ()
link_directories(${CMAKE_SOURCE_DIR}/Lib)

add_compile_definitions(Z_PLATFORM_WIN32)
add_compile_definitions(Z_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
include_directories(${CMAKE_SOURCE_DIR}/Engine/Include/spdlog/include
        ${CMAKE_SOURCE_DIR}/Engine/ ${CMAKE_SOURCE_DIR}/Engine/Include/glm
        ${CMAKE_SOURCE_DIR}/Engine/Include
        ${ImGuiPath}
        ${enttPath}
        ${yamlPath}
        ${ImGuizmoPath}
        depends)
add_subdirectory(Engine/Include/GLFW)
add_subdirectory(Engine/Include/glad)
add_subdirectory(Engine/Include/imgui)
add_subdirectory(Engine/Include/yaml-cpp)

add_subdirectory(Engine)

add_dependencies(Z glad GLFW ImGui)
#add_subdirectory(Test)

add_subdirectory(ZEditor)
