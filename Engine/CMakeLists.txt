cmake_minimum_required(VERSION 3.21)

project(ZCore)

if(ReBuildScriptsCore)
    add_subdirectory(ZScriptsCore)
endif ()

if(NOT UseVulkan)
    set(Z_INCLUDE_DIR   ${Z_INCLUDE_DIR}
                        ${gladPath}/include)
    add_subdirectory(Include/glad)
endif ()

set(Z_INCLUDE_DIR   ${Z_INCLUDE_DIR}
                    ${spdlogPath}/include
                    ${GLFWPath}/include
                    ${glmPath}
                    ${ImGuiPath}
                    ${ImGuiPath}/backends
                    ${monoPath}/include
                    ${VK_PATH}/include
                    ${box2DPath}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${yamlPath})


add_subdirectory(Include/glfw)
add_subdirectory(Include/yaml-cpp)
add_subdirectory(Include/box2d)
add_subdirectory(Include/tinyobjloader)
add_subdirectory(Include/spdlog)


file(GLOB_RECURSE src CONFIGURE_DEPENDS Z/*.*
                                        Platform/OpenGL/*.*
                                        Platform/Vulkan/*.*
                                        Platform/${Z_PLATFORM}/*.*
                                        ${ImGuizmoPath}/ImGuizmo.cpp
                                        ${ImGuiPath}/*.cpp)


set(Z_LINK_DIR ${Z_LINK_DIR}
               ${VK_PATH}/lib
               ${CMAKE_SOURCE_DIR}/Lib/${Z_BUILD_TYPE}
               ${monoPath}/lib)

if(Z_LIBRARY_TYPE STREQUAL "SHARED")
    add_library(${PROJECT_NAME} SHARED ${src})
    target_compile_definitions(${PROJECT_NAME} PUBLIC Z_DYNAMIC_LINK)
    target_compile_definitions(${PROJECT_NAME} PRIVATE  Z_BUILD_DLL)
else ()
    add_library(${PROJECT_NAME} STATIC ${src})
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE ${Z_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${Z_LINK_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
        monosgen-2.0 
        glfw yaml box2d tinyobjloader spdlog
        )

if(${Z_BUILD_TYPE} MATCHES Debug AND CMAKE_HOST_WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE shadercd shaderc_combinedd  SPIRV-Toolsd
                            spirv-cross-glsld spirv-cross-cored)
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE shaderc shaderc_combined  SPIRV-Tools
                            spirv-cross-glsl spirv-cross-core)
endif ()

if(NOT UseVulkan)
    target_link_libraries(${PROJECT_NAME} PRIVATE glad)
endif ()

if (CMAKE_HOST_LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE vulkan)
elseif (CMAKE_HOST_WIN32)
    # noticed that vulkan lib named different on Windows
    target_link_libraries(${PROJECT_NAME} PRIVATE vulkan-1
                                    Ws2_32 winmm Version Bcrypt)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
else ()
    message(FATAL_ERROR "not supported platform : ${CMAKE_HOST_SYSTEM_NAME}")
endif ()


if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -w)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W0)
endif()

add_definitions(-DZ_ENABLE_PROFILING=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        ${PLATFORM_SYMBOL}
        Z_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        ${RENDERER_BACKEND_FLAG}
        GLM_ENABLE_EXPERIMENTAL
        Z_CONF_EXTENSION=".zConf"
        Z_PLATFORM_${Z_WM_FLAG}
        GLM_FORCE_DEPTH_ZERO_TO_ONE)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)

if (Z_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC Z_DEBUG
                                        Z_ENABLE_PROFILING=1)
endif ()

if(UseVulkan)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_USE_VULKAN VK_USE_PLATFORM_${Z_WM_FLAG}_KHR)
endif ()

if(UseSecureGUID)
    target_compile_definitions(${PROJECT_NAME} PRIVATE Z_SECUREGUID)
endif ()
