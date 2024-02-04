cmake_minimum_required(VERSION 3.21)

project(ZEditor)


include_directories(${CMAKE_SOURCE_DIR}/Engine
                    ${glmPath}
                    ${spdlogPath}/include
                    depends)

file(GLOB_RECURSE src CONFIGURE_DEPENDS *.cpp *.h)
link_directories(${VK_PATH}/lib)

add_executable(${PROJECT_NAME} ${src} ${EDITOR_RC})
if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC depends)
target_link_libraries(${PROJECT_NAME} PUBLIC Z)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DZ_ENABLE_PROFILING=1)
    target_compile_definitions(${PROJECT_NAME} PUBLIC Z_DEBUG Z_ENABLE_PROFILING)
endif ()