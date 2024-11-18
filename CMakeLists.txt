cmake_minimum_required (VERSION 3.6)

project(lerw)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -O3 -fconcepts-diagnostics-depth=4 -march=native")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(lerw src/main.cpp)

target_link_libraries(lerw PRIVATE
  tbb
  boost_program_options)

# nix-build wants an 'install' target
install(TARGETS lerw DESTINATION .)
