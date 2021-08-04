cmake_minimum_required(VERSION 3.18)

project(
  TerraGen
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.[ch]pp")
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

add_subdirectory(SDL)
add_subdirectory(fmt)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main fmt::fmt)
target_include_directories(${PROJECT_NAME} PRIVATE "deps/FastNoiseLite")
