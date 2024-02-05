cmake_minimum_required(VERSION 3.21)

project(ZEditor)


set(EDITOR_INCLUDE_DIR  ${EDITOR_INCLUDE_DIR}
                        ${CMAKE_SOURCE_DIR}/Engine
                        ${glmPath}
                        ${spdlogPath}/include
                        )

file(GLOB_RECURSE src CONFIGURE_DEPENDS *.cpp *.h)

add_executable(${PROJECT_NAME} ${src} ${EDITOR_RC})
if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${EDITOR_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Z)
