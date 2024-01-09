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



add_subdirectory(Include/glfw)
add_subdirectory(Include/glad)
add_subdirectory(Include/imgui)
add_subdirectory(Include/yaml-cpp)
add_subdirectory(Include/box2d)
add_subdirectory(Include/tinyobjloader)

add_compile_definitions(GLFW_INCLUDE_NONE)

file(GLOB_RECURSE src CONFIGURE_DEPENDS Z/*.* Platform/*.* ${ImGuizmoPath}/ImGuizmo.cpp)


link_directories(${VK_PATH}/lib ${CMAKE_SOURCE_DIR}/Lib/${CMAKE_BUILD_TYPE})
add_library(Z STATIC ${src})


target_include_directories(Z PUBLIC depends)
#Todo: change
target_link_libraries(Z PUBLIC
        glfw glad imgui yaml Box2D tinyobjloader
        vulkan shaderc_shared spirv-cross-core spirv-cross-glsl
        )

if (CMAKE_HOST_LINUX)
    # build by pls on arch 6.6.10 zen kernel
    # with mono source : https://github.com/mono/mono
    target_link_libraries(Z PUBLIC monosgen-2.0)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        # needed by monosgen for symbol "inflate" and "inflateInit2_" ?
        find_package(ZLIB REQUIRED)
        target_link_libraries(Z PUBLIC ZLIB::ZLIB)
    endif ()
elseif (CMAKE_HOST_WIN32)
    target_link_libraries(Z PUBLIC libmono-static-sgen
                                    Ws2_32 winmm Version Bcrypt)
else ()
    message(FATAL_ERROR "not supported platform : ${CMAKE_HOST_SYSTEM_NAME}")
endif ()


add_definitions(-DZ_ENABLE_PROFILING=1)

target_compile_definitions(Z PUBLIC
        ${PLATFORM_SYMBOL}
        Z_SOURCE_DIR
        Z_USE_OPENGL
        GLFW_INCLUDE_NONE
        Z_CONF_EXTENSION=".zConf")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Core:Debug mode")
    target_compile_definitions(Z PUBLIC Z_DEBUG
                                        Z_ENABLE_PROFILING)
endif ()