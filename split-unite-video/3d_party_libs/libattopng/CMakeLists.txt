cmake_minimum_required(VERSION 3.14)
project(libattopng)

add_library(libattopng STATIC libattopng.c libattopng.h)

target_include_directories(libattopng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})