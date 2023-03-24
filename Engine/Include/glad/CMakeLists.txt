cmake_minimum_required(VERSION 3.22)

project(glad)
include_directories(include)
add_library(glad STATIC
    src/glad.c
)
target_include_directories(glad PUBLIC /include)