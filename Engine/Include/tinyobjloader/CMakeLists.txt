cmake_minimum_required(VERSION 3.22)

project(tinyobjloader)
if(TINYOBJLOADER_LIBRARY_TYPE STREQUAL "SHARED")
    add_library(${PROJECT_NAME} SHARED tiny_obj_loader.cc tiny_obj_loader.h)
else ()
    add_library(${PROJECT_NAME} STATIC tiny_obj_loader.cc tiny_obj_loader.h)
endif ()