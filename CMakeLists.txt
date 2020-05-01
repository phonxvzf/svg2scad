cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(svg2scad)

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

find_package(Boost COMPONENTS system filesystem REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++14 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++14 -march=native -O2 -flto")

add_executable(svg2scad ${SOURCES})

target_include_directories(svg2scad PRIVATE include)
target_link_libraries(svg2scad PRIVATE ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
