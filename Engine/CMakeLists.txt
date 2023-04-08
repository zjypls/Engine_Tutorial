cmake_minimum_required(VERSION 3.22)
project(Z)

include_directories(${CMAKE_SOURCE_DIR}/Engine/Include/spdlog/include depends)
include_directories(Include/GLFW/include depends)
include_directories(Include/glm depends)
include_directories(Include/glad/include depends)
include_directories(${ImGuiPath}/backends depends)
include_directories(Include/mono/include depends)

include_directories($ENV{VK_SDK_PATH}/Include depends)
include_directories(Include/box2d/include depends)


add_compile_definitions(GLFW_INCLUDE_NONE)
file(GLOB src CONFIGURE_DEPENDS
        Z/Core/*.h Z/Core/*.cpp
        Platform/Windows/*.cpp Platform/Windows/*.h
        Platform/OpenGL/*.cpp Platform/OpenGL/*.h
        Z/Events/*.h
        Z/ImGui/*.h
        Z/ImGui/*.cpp
        Z/Renderer/*.h
        Z/Renderer/*.cpp
        Z/Debug/*.h Z/Debug/*.cpp
        Z/Scene/*.h Z/Scene/*.cpp
        Z/Utils/*.h
        Z/Script/*.h Z/Script/*.cpp
        ${ImGuizmoPath}/ImGuizmo.h ${ImGuizmoPath}/ImGuizmo.cpp)


add_library(Z STATIC ${src})


target_include_directories(Z PUBLIC depends)
#Todo: change
target_link_libraries(Z PUBLIC
        GLFW glad ImGui yaml Box2D
        shaderc_sharedd spirv-cross-cored spirv-cross-glsld
        libmono-static-sgen
        Ws2_32 winmm Version Bcrypt
        )



target_compile_definitions(Z PUBLIC Z_PLATFORM_WIN32 GLFW_INCLUDE_NONE Z_SOURCE_DIR)