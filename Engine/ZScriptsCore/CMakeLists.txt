cmake_minimum_required(VERSION 3.20)

project(ScriptsCore LANGUAGES CSharp)

if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin>)
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif ()
file(GLOB SOURCES src/*.cs src/Engine/*.cs)
add_library(ScriptsCore SHARED ${SOURCES})
target_link_libraries(ScriptsCore PUBLIC System)
