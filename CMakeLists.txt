cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)

find_package(Git)
if(GIT_FOUND)
  # Update submodules as needed
  option(GIT_SUBMODULE "Check submodules during build" ON)
  if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    message(STATUS "Git executable: '${GIT_EXECUTABLE}'")
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" submodule update --init --recursive
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      RESULT_VARIABLE GIT_SUBMOD_RESULT
    )
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(
        FATAL_ERROR
          "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules"
      )
    endif()
  endif()
endif()

include(cmake/CPM.cmake)
include(FetchContent)

cpmaddpackage(
  NAME
  Format.cmake
  VERSION
  1.7.0
  GITHUB_REPOSITORY
  TheLartians/Format.cmake
  OPTIONS # set to yes skip cmake formatting
  "FORMAT_SKIP_CMAKE NO"
  # path to exclude (optional, supports regular expressions)
  "CMAKE_FORMAT_EXCLUDE cmake/CPM.cmake"
)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.0.14
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 8.0.1
)
FetchContent_MakeAvailable(fmt)

project(
  TerraGen
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.[ch]pp")
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main fmt::fmt)
target_include_directories(
  ${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/deps/FastNoiseLite"
)
