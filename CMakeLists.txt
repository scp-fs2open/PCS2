cmake_minimum_required(VERSION 3.0)


LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(PCS2)

include(toolchain)
include(platforms)

add_subdirectory(src)
