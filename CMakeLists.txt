cmake_minimum_required(VERSION 3.24)

project(EngineAll)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-w)

set(DynamicLink OFF)
#this flags open mean msvc generator only because cmake project with C# language only support msvc generator
set(ReBuildScriptsCore ON)
set(UseVulkan OFF)

set(gladPath ${CMAKE_SOURCE_DIR}/Engine/Include/glad)
set(GLFWPath ${CMAKE_SOURCE_DIR}/Engine/Include/glfw)
set(ImGuiPath ${CMAKE_SOURCE_DIR}/Engine/Include/imgui)
set(enttPath ${CMAKE_SOURCE_DIR}/Engine/Include/entt/include)
set(yamlPath ${CMAKE_SOURCE_DIR}/Engine/Include/yaml-cpp/include)
set(ImGuizmoPath ${CMAKE_SOURCE_DIR}/Engine/Include/ImGuizmo)
set(monoPath ${CMAKE_SOURCE_DIR}/Engine/Include/mono)
set(glmPath ${CMAKE_SOURCE_DIR}/Engine/Include/glm)
set(spdlogPath ${CMAKE_SOURCE_DIR}/Engine/Include/spdlog)
set(box2DPath ${CMAKE_SOURCE_DIR}/Engine/Include/box2d)

if (DEFINED $ENV{VULKAN_SDK})
    set(VK_PATH $ENV{VULKAN_SDK})
elseif (DEFINED $ENV{VK_SDK_PATH})
    set(VK_PATH $ENV{VK_SDK_PATH})
else ()
    set(VK_PATH ${CMAKE_SOURCE_DIR}/Engine/Include/vulkanSDK)
endif ()

set(EDITOR_RC ${CMAKE_SOURCE_DIR}/Assets/Configs/AppIcon.rc)


set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib/${CMAKE_BUILD_TYPE})
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/PDBs/${CMAKE_BUILD_TYPE})

if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ZEditor)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/Bin-C>)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin-C)
endif ()


link_directories(Engine/Include/mono/lib/${CMAKE_BUILD_TYPE})
add_compile_definitions(Z_PLATFORM_WIN32)
add_compile_definitions(Z_SOURCE_DIR="${CMAKE_SOURCE_DIR}")


add_subdirectory(Engine)

add_dependencies(Z glad glfw imgui Box2D)
add_subdirectory(ZEditor)
if (ReBuildScriptsCore)
    add_subdirectory(scripts)
    add_dependencies(scripts ScriptCore)
endif ()

