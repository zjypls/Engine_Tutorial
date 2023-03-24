cmake_minimum_required(VERSION 3.22)

project(Test)


file(GLOB src CONFIGURE_DEPENDS *.cpp *.h)

add_executable(${PROJECT_NAME} ${src} SanBox2D.cpp SanBox2D.h)
target_include_directories(${PROJECT_NAME} PUBLIC depends)
target_link_libraries(${PROJECT_NAME} PUBLIC Z)
target_compile_definitions(${PROJECT_NAME} PUBLIC Z_PLATFORM_WIN32)
if (ImGui_Docking)
target_compile_definitions(Z PUBLIC Z_IMGUI_ENABLE_DOCKING)
endif ()
if (DynamicLink)
target_compile_definitions(Z PUBLIC Z_DYNAMIC_LINK)
endif ()