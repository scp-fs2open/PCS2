
MESSAGE(STATUS "Doing configuration specific to gcc...")

unset(CMAKE_CXX_FLAGS)
if(DEFINED ENV{CXXFLAGS})
	set(CMAKE_CXX_FLAGS $ENV{CXXFLAGS})
	message("${CMAKE_CXX_FLAGS}")
endif()

if(NOT CMAKE_CXX_FLAGS)
	set(CMAKE_CXX_FLAGS "-march=native -pipe")
endif(NOT CMAKE_CXX_FLAGS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -funroll-loops -fsigned-char -Wno-unknown-pragmas")

# Omit "deprecated conversion from string constant to 'char*'" warnings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-char-subscripts")

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wno-unused-variable -Wno-unused-but-set-variable -Wno-array-bounds")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wextra -Wshadow -Wno-unused-parameter")

if (FSO_FATAL_WARNINGS)
	# Make warnings fatal if the right variable is set
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

set(CMAKE_EXE_LINKER_FLAGS "")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-g -rdynamic")

IF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
	SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-zignore")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
