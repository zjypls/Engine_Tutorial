cmake_minimum_required(VERSION 3.22)


project(imgui)

file(GLOB src *.cpp *.h)

if(IMGUI_LIBRARY_TYPE STREQUAL "SHARED")
    add_library(${PROJECT_NAME} SHARED ${src} backends/imgui_impl_glfw.cpp)
else ()
    add_library(${PROJECT_NAME} STATIC ${src} backends/imgui_impl_glfw.cpp)
endif ()
