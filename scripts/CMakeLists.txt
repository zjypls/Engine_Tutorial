cmake_minimum_required(VERSION 3.22)


project(scripts CSharp)


file(GLOB srcs *.cs)

add_library(scripts SHARED ${srcs})
target_link_libraries(scripts ScriptCore)
