cmake_minimum_required(VERSION 3.18)

find_package(Git)
if(GIT_FOUND)
  # Update submodules as needed
  option(GIT_SUBMODULE "Check submodules during build" ON)
  if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" submodule update --init --recursive
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(
        FATAL_ERROR
          "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules"
      )
    endif()
  endif()
endif()

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
