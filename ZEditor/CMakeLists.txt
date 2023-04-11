cmake_minimum_required(VERSION 3.22)

project(ZEditor)

add_subdirectory(scripts)

file(GLOB_RECURSE src CONFIGURE_DEPENDS *.cpp *.h)
add_executable(${PROJECT_NAME} ${src})
if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(TARGET ZEditor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()
add_dependencies(${PROJECT_NAME} scripts)
target_include_directories(${PROJECT_NAME} PUBLIC depends ${ImGuiPath}/backends/)
target_link_libraries(${PROJECT_NAME} PUBLIC Z)
target_compile_definitions(${PROJECT_NAME} PUBLIC Z_PLATFORM_WIN32 Z_SOURCE_DIR)

if (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    message("ZEditor: Debug mode")
    target_compile_definitions(${PROJECT_NAME} PUBLIC Z_DEBUG)
endif ()