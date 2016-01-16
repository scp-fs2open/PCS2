
INCLUDE(util)

MESSAGE(STATUS "Configuring UNIX specific things and stuff...")

target_compile_definitions(platform INTERFACE UNIX HAVE_SYS_TYPES_H HAVE_SYS_STAT_H HAVE_UNISTD_H)
