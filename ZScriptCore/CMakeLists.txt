cmake_minimum_required(VERSION 3.20)

project(ScriptCore LANGUAGES CSharp)


set(CMAKE_CShairp_COMPILER $ENV{VISUAL_STUDIO_PATH}/MSBuild/Current/Bin/MSBuild.exe)
file(GLOB SOURCES src/*.cs src/Engine/*.cs)
add_library(ScriptCore SHARED ${SOURCES})
target_link_libraries(ScriptCore PUBLIC System)
