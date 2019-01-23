cmake_minimum_required(VERSION 3.5)
project(tdata)

set(CMAKE_CXX_STANDARD 11)

add_executable(tdata include/variant.hpp include/tdata.hpp test/main.cc)

