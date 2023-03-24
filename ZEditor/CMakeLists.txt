cmake_minimum_required(VERSION 3.22)

project(ZEditor)


file(GLOB src CONFIGURE_DEPENDS *.cpp *.h)

add_executable(${PROJECT_NAME} ${src})
target_include_directories(${PROJECT_NAME} PUBLIC depends ${ImGuiPath}/backends/)
target_link_libraries(${PROJECT_NAME} PUBLIC Z)
target_compile_definitions(${PROJECT_NAME} PUBLIC Z_PLATFORM_WIN32 Z_SOURCE_DIR)
