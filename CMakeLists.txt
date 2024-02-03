cmake_minimum_required(VERSION 3.24)

project(EngineAll)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DynamicLink OFF)

if(DynamicLink)
    message(WARNING "Build shared engine lib !")
    set(Z_LIBRARY_TYPE             "SHARED")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()

#this flags open mean msvc generator only because cmake project with C# language only support msvc generator
set(ReBuildScriptsCore OFF)
set(UseVulkan ON)

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
set(EDITOR_RC ${CMAKE_SOURCE_DIR}/Assets/Configs/AppIcon.rc)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib/${CMAKE_BUILD_TYPE})
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/PDBs/${CMAKE_BUILD_TYPE})

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)


if (DEFINED ENV{VULKAN_SDK})
    set(VK_PATH $ENV{VULKAN_SDK})
elseif (DEFINED ENV{VK_SDK_PATH})
    set(VK_PATH $ENV{VK_SDK_PATH})
else ()
    # noticed that some IDE like Jetbrains CLion will not load environment that export by .bashrc on Linux
    message(FATAL_ERROR "Vulkan SDK not found and you should expose it by set environment value like VULKAN_SDK")
endif ()

if(UseVulkan)
    set(RENDERER_BACKEND Vulkan)
    set(RENDERER_BACKEND_FLAG Z_USE_VULKAN)
else ()
    set(RENDERER_BACKEND OpenGL)
    set(RENDERER_BACKEND_FLAG Z_USE_OPENGL)
endif ()


if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ZEditor)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin>)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else ()
    add_definitions(-w)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif ()


link_directories(Engine/Include/mono/lib/${CMAKE_BUILD_TYPE})
if(CMAKE_HOST_WIN32)
    set(PLATFORM_SYMBOL Z_PLATFORM_WIN32)
    set(Z_PLATFORM Windows)
    set(Z_WM_FLAG WIN32)
elseif(CMAKE_HOST_LINUX)
    set(PLATFORM_SYMBOL Z_PLATFORM_LINUX)
    set(Z_PLATFORM Linux)
    if(FALSE AND (DEFINED ENV{WAYLAND_DISPLAY} OR (DEFINED ENV{XDG_SESSION_TYPE} AND $ENV{XDG_SESSION_TYPE} MATCHES wayland)))
        message(WARNING "Using Wayland for linux ! If not, change it !")
        # noticed that glfw-wayland always error call the resize func after the first frame submit
        # that give twice the original size to application and this make application give vulkan an error frame extent
        # using x11 always before find out why
        set(Z_WM_FLAG WAYLAND)
        set(GLFW_USE_WAYLAND=1)
        set(GLFW_BUILD_WAYLAND ON)
        set(GLFW_BUILD_X11 OFF)
    else ()
        set(Z_WM_FLAG X11)
        set(GLFW_USE_X11=1)
        set(GLFW_BUILD_WAYLAND OFF)
        set(GLFW_BUILD_X11 ON)
    endif ()
else ()
    message(FATAL_ERROR "not support at ${CMAKE_HOST_SYSTEM_NAME}")
endif ()

message("Using Backend API : ${RENDERER_BACKEND}")
message("Host Platform : ${Z_PLATFORM}")
message("Set Rebuild Scripts : ${ReBuildScriptsCore}")
message("Build Type : ${CMAKE_BUILD_TYPE}")


add_subdirectory(Engine)

add_subdirectory(ZEditor)
if (ReBuildScriptsCore)
    add_subdirectory(Projects/Test001/Assets/scripts)
endif ()