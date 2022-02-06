# Needs to be changed appropriately
cmake_minimum_required(VERSION 3.12)
project(fdtd CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads)
find_package(PkgConfig REQUIRED)
PKG_CHECK_MODULES(RAFTLIB REQUIRED IMPORTED_TARGET raftlib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)")

aux_source_directory(src/rl RL_SOURCES)
aux_source_directory(src/norl NORL_SOURCES)
aux_source_directory(src/common COMMON_SOURCES)

add_executable(fdtd-rl ${RL_SOURCES} ${COMMON_SOURCES})
add_executable(fdtd-norl ${NORL_SOURCES} ${COMMON_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# RL Version
target_include_directories(fdtd-rl PRIVATE src/common)
target_link_libraries(fdtd-rl PRIVATE PkgConfig::RAFTLIB ${CMAKE_THREAD_LIBS_INIT})

# NoRL Version
target_include_directories(fdtd-norl PRIVATE src/common)
target_link_libraries(fdtd-norl PRIVATE ${CMAKE_THREAD_LIBS_INIT})
