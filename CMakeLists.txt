cmake_minimum_required(VERSION 3.0)

include(cmake/util.cmake)

# Set a few policies
set_policy(CMP0051 OLD)
set_policy(CMP0053 NEW)
set_policy(CMP0054 NEW)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/external/cmake-modules")

project(PCS2)

include(toolchain)
include(platforms)

add_subdirectory(src)
