cmake_minimum_required(VERSION 3.21)

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
                    ${yamlPath}
                    depends)


add_subdirectory(Include/glfw)
add_subdirectory(Include/yaml-cpp)
add_subdirectory(Include/box2d)
add_subdirectory(Include/tinyobjloader)
add_subdirectory(Include/spdlog)

add_compile_definitions(GLFW_INCLUDE_NONE)

file(GLOB_RECURSE src CONFIGURE_DEPENDS Z/*.*
                                        Platform/OpenGL/*.*
                                        Platform/Vulkan/*.*
                                        Platform/${Z_PLATFORM}/*.*
                                        ${ImGuizmoPath}/ImGuizmo.cpp
                                        ${ImGuiPath}/*.cpp)


link_directories(${VK_PATH}/lib ${CMAKE_SOURCE_DIR}/Lib/${Z_BUILD_TYPE} ${monoPath}/lib)

if(Z_LIBRARY_TYPE STREQUAL "SHARED")
    add_library(Z SHARED ${src})
    target_compile_definitions(Z PUBLIC Z_DYNAMIC_LINK)
    target_compile_definitions(Z PRIVATE  Z_BUILD_DLL)
else ()
    add_library(Z STATIC ${src})
endif ()

target_include_directories(Z PUBLIC depends)

target_link_libraries(Z PUBLIC
        monosgen-2.0
        glfw yaml box2d tinyobjloader spdlog
        spirv-cross-glsl
        )

if(${Z_BUILD_TYPE} MATCHES Debug AND CMAKE_HOST_WIN32)
    target_link_libraries(Z PUBLIC shadercd shaderc_combinedd  SPIRV-Toolsd)
else ()
    target_link_libraries(Z PUBLIC shaderc shaderc_combined  SPIRV-Tools)
endif ()

if(NOT UseVulkan)
    target_link_libraries(Z PUBLIC glad)
endif ()

if (CMAKE_HOST_LINUX)
    # build by pls with gcc13 on Arch 6.7 zen kernel
    # with mono source : https://github.com/mono/mono
    find_package(   ZLIB # needed by monosgen for symbols "inflate" and "inflateInit2_"
                    REQUIRED)
    target_link_libraries(Z PUBLIC vulkan ZLIB::ZLIB)
elseif (CMAKE_HOST_WIN32)
    # noticed that vulkan lib named different on Windows
    target_link_libraries(Z PUBLIC vulkan-1
                                    Ws2_32 winmm Version Bcrypt)
else ()
    message(FATAL_ERROR "not supported platform : ${CMAKE_HOST_SYSTEM_NAME}")
endif ()


# Disable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Z PRIVATE -w)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(Z PRIVATE /W0)
endif()

add_definitions(-DZ_ENABLE_PROFILING=1)

target_compile_definitions(Z PUBLIC
        ${PLATFORM_SYMBOL}
        Z_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        ${RENDERER_BACKEND_FLAG}
        GLM_ENABLE_EXPERIMENTAL
        Z_CONF_EXTENSION=".zConf")

target_compile_definitions(Z PRIVATE GLFW_INCLUDE_NONE)

if (Z_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(Z PUBLIC Z_DEBUG
                                        Z_ENABLE_PROFILING)
endif ()

if(UseVulkan)
    target_compile_definitions(Z PUBLIC GLFW_USE_VULKAN VK_USE_PLATFORM_${Z_WM_FLAG}_KHR)
endif ()

if(UseSecureGUID)
    target_compile_definitions(Z PRIVATE Z_SECUREGUID)
endif ()