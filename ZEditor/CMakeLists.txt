cmake_minimum_required(VERSION 3.22)

project(ZEditor)


include_directories(${CMAKE_SOURCE_DIR}/Engine
                    ${glmPath}
                    ${spdlogPath}/include
                    depends)

file(GLOB_RECURSE src CONFIGURE_DEPENDS *.cpp *.h)
link_directories(${VK_PATH}/Lib)

add_executable(${PROJECT_NAME} ${src})
if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC depends)
target_link_libraries(${PROJECT_NAME} PUBLIC Z)
target_compile_definitions(${PROJECT_NAME} PUBLIC Z_PLATFORM_WIN32 Z_SOURCE_DIR Z_CONF_EXTENSION=".zConf")

if (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    message("ZEditor: Debug mode")
    target_compile_definitions(${PROJECT_NAME} PUBLIC Z_DEBUG)
endif ()