
# File to execute compiler specific commands

add_library(compiler INTERFACE)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	include(toolchain-gcc)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	include(toolchain-msvc)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	include(toolchain-clang)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	include(toolchain-apple-clang)
ELSE("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	MESSAGE(STATUS "No special handling for this compiler present, good luck!")
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")