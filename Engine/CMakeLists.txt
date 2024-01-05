cmake_minimum_required(VERSION 3.22)

project(Z)

if(ReBuildScriptsCore)
    add_subdirectory(ZScriptCore)
endif ()

include_directories(${spdlogPath}/include
                    ${GLFWPath}/include
                    ${glmPath}
                    ${gladPath}/include
                    ${ImGuiPath}
                    ${ImGuiPath}/backends
                    ${monoPath}/include
                    ${VK_PATH}/include
                    ${box2DPath}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    depends)



add_subdirectory(Include/GLFW)
add_subdirectory(Include/glfw)
add_subdirectory(Include/glad)
add_subdirectory(Include/imgui)
add_subdirectory(Include/yaml-cpp)
add_subdirectory(Include/Box2D)
add_subdirectory(Include/tinyobjloader)

add_compile_definitions(GLFW_INCLUDE_NONE)

file(GLOB_RECURSE src CONFIGURE_DEPENDS Z/*.* Platform/*.* ${ImGuizmoPath}/ImGuizmo.cpp)


link_directories(${VK_PATH}/lib ${CMAKE_SOURCE_DIR}/Lib/${CMAKE_BUILD_TYPE})
add_library(Z STATIC ${src})


target_include_directories(Z PUBLIC depends)
#Todo: change
target_link_libraries(Z PUBLIC
        glfw glad imgui yaml Box2D tinyobjloader
        vulkan-1 shaderc_shared spirv-cross-core spirv-cross-glsl
        libmono-static-sgen
        Ws2_32 winmm Version Bcrypt
        )
add_definitions(-DZ_ENABLE_PROFILING=1)

target_compile_definitions(Z PUBLIC
        Z_PLATFORM_WIN32 Z_SOURCE_DIR Z_USE_OPENGL
        Z_ENABLE_PROFILING
        GLFW_INCLUDE_NONE
        Z_CONF_EXTENSION=".zConf")

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("Core:Debug mode")
    target_compile_definitions(Z PUBLIC Z_DEBUG)
endif ()