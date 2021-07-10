cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)

project(
  noise
  VERSION 1.0.0
  LANGUAGES CXX)
file(GLOB SOURCES CONFIGURE_DEPENDS "noise/src/*.cpp")
file(GLOB MODULE_SOURCES CONFIGURE_DEPENDS "noise/src/module/*.cpp")
file(GLOB MODEL_SOURCES CONFIGURE_DEPENDS "noise/src/model/*.cpp")
if(WIN32)
  file(GLOB WIN32_SOURCES CONFIGURE_DEPENDS "noise/src/win32/*.cpp")
endif()
add_library(noise ${SOURCES} ${MODEL_SOURCES} ${MODULE_SOURCES}
                  ${WIN32_SOURCES})
target_include_directories(noise PUBLIC "noise/src")

project(
  TerraGen
  VERSION 0.1.0
  LANGUAGES CXX)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.[ch]pp")
add_executable(${PROJECT_NAME} ${SOURCES})

find_package(SDL2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2 fmt::fmt noise)
