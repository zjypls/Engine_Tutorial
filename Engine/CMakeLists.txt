cmake_minimum_required(VERSION 3.22)

project(Z)

if(ReBuildScriptsCore)
    add_subdirectory(ZScriptsCore)
endif ()

if(NOT UseVulkan)
    include_directories(${gladPath}/include)
    add_subdirectory(Include/glad)
endif ()

include_directories(${spdlogPath}/include
                    ${GLFWPath}/include
                    ${glmPath}
                    ${ImGuiPath}
                    ${ImGuiPath}/backends
                    ${monoPath}/include
                    ${VK_PATH}/include
                    ${box2DPath}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    depends)


add_subdirectory(Include/glfw)
add_subdirectory(Include/imgui)
add_subdirectory(Include/yaml-cpp)
add_subdirectory(Include/box2d)
add_subdirectory(Include/tinyobjloader)

add_compile_definitions(GLFW_INCLUDE_NONE)

file(GLOB_RECURSE src CONFIGURE_DEPENDS Z/*.*
                                        Platform/OpenGL/*.*
                                        Platform/Vulkan/*.*
                                        Platform/${Z_PLATFORM}/*.*
                                        ${ImGuizmoPath}/ImGuizmo.cpp)


link_directories(${VK_PATH}/lib ${CMAKE_SOURCE_DIR}/Lib/${CMAKE_BUILD_TYPE})
add_library(Z STATIC ${src})


target_include_directories(Z PUBLIC depends)

target_link_libraries(Z PUBLIC
        glfw imgui yaml Box2D tinyobjloader
        spirv-cross-glsl
        )
if(${CMAKE_BUILD_TYPE} MATCHES Debug AND CMAKE_HOST_WIN32)
    target_link_libraries(Z PUBLIC shadercd shaderc_combinedd  SPIRV-Toolsd)
else ()
    target_link_libraries(Z PUBLIC shaderc shaderc_combined  SPIRV-Tools)
endif ()
if(NOT UseVulkan)
    target_link_libraries(Z PUBLIC glad)
endif ()

if (CMAKE_HOST_LINUX)
    # build by pls on arch 6.6.10 zen kernel
    # with mono source : https://github.com/mono/mono
    message(WARNING "Suppose to use system mono")
    find_package(   ZLIB # needed by monosgen for symbols "inflate" and "inflateInit2_"
                    REQUIRED)
    target_link_libraries(Z PUBLIC vulkan monosgen-2.0 ZLIB::ZLIB)
elseif (CMAKE_HOST_WIN32)
    # noticed that vulkan lib named different on Windows
    target_link_libraries(Z PUBLIC vulkan-1 libmono-static-sgen
                                    Ws2_32 winmm Version Bcrypt)
else ()
    message(FATAL_ERROR "not supported platform : ${CMAKE_HOST_SYSTEM_NAME}")
endif ()


add_definitions(-DZ_ENABLE_PROFILING=1)

target_compile_definitions(Z PUBLIC
        ${PLATFORM_SYMBOL}
        Z_SOURCE_DIR
        ${RENDERER_BACKEND_FLAG}
        GLFW_INCLUDE_NONE
        Z_CONF_EXTENSION=".zConf")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(Z PUBLIC Z_DEBUG
                                        Z_ENABLE_PROFILING)
endif ()