cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)

project(
  TerraGen
  VERSION 0.1.0
  LANGUAGES CXX)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.[ch]pp")
add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(SDL)

find_package(fmt REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main fmt::fmt)
target_include_directories(${PROJECT_NAME} PRIVATE "deps/FastNoiseLite")
