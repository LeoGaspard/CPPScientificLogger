cmake_minimum_required(VERSION 3.19)

project(Logger LANGUAGES CXX)

add_library(logger INTERFACE)
target_include_directories(logger INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
