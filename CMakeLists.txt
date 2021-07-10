cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)

project(
  TerraGen
  VERSION 0.1.0
  LANGUAGES CXX)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.[ch]pp")
add_executable(${PROJECT_NAME} ${SOURCES})

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2)
