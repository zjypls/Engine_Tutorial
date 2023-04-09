cmake_minimum_required(VERSION 3.20)

project(Script LANGUAGES CSharp)


set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION "v4.8")
set(CMAKE_CShairp_COMPILER $ENV{VISUAL_STUDIO_PATH}/MSBuild/Current/Bin/MSBuild.exe)
file(GLOB SOURCES src/*.cs src/Engine/*.cs)
add_library(Script SHARED ${SOURCES})
target_link_libraries(Script PUBLIC System)
