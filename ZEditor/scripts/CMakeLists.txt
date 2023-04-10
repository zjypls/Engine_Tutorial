cmake_minimum_required(VERSION 3.22)


project(scripts CSharp)

file(GLOB srcs *.cs)

add_library(scripts SHARED ${srcs})
add_dependencies(scripts ScriptCore)
target_link_libraries(scripts ${CMAKE_SOURCE_DIR}/MSVC)
