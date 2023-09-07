cmake_minimum_required(VERSION 3.22)
#set(CMAKE_BUILD_TYPE RELEASE)
project(Z)

include_directories(${CMAKE_SOURCE_DIR}/Engine/Include/spdlog/include depends)
include_directories(Include/GLFW/include depends)
include_directories(Include/glm depends)
include_directories(Include/glad/include depends)
include_directories(${ImGuiPath}/backends depends)
include_directories(Include/mono/include depends)

include_directories($ENV{VK_SDK_PATH}/Include depends)
include_directories(Include/box2d/include depends)
link_directories($ENV{VK_SDK_PATH}/Lib)


add_compile_definitions(GLFW_INCLUDE_NONE)
file(GLOB src CONFIGURE_DEPENDS
        Z/Core/*.h Z/Core/*.cpp
        Platform/Windows/*.cpp Platform/Windows/*.h
        Platform/OpenGL/*.cpp Platform/OpenGL/*.h
        Platform/Vulkan/*.cpp Platform/Vulkan/*.h
        Z/Events/*.h
        Z/ImGui/*.h
        Z/ImGui/*.cpp
        Z/Renderer/*.h
        Z/Renderer/*.cpp
        Z/Debug/*.h Z/Debug/*.cpp
        Z/Scene/*.h Z/Scene/*.cpp Z/Scene/*.hpp
        Z/Utils/*.h
        Z/Script/*.h Z/Script/*.cpp
        ${ImGuizmoPath}/ImGuizmo.h ${ImGuizmoPath}/ImGuizmo.cpp
        Z/Project/*.h Z/Project/*.cpp)


add_library(Z STATIC ${src})


target_include_directories(Z PUBLIC depends)
#Todo: change
target_link_libraries(Z PUBLIC
        GLFW glad ImGui yaml Box2D
        vulkan-1 shaderc_shared spirv-cross-core spirv-cross-glsl
        libmono-static-sgen
        Ws2_32 winmm Version Bcrypt
        )


target_compile_definitions(Z PUBLIC Z_PLATFORM_WIN32 GLFW_INCLUDE_NONE Z_SOURCE_DIR Z_ENABLE_VK_VALIDATION Z_USE_OPENGL)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("Core:Debug mode")
    target_compile_definitions(Z PUBLIC Z_DEBUG)
endif ()