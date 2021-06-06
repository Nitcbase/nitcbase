cmake_minimum_required(VERSION 3.19)
project(NITCbase)

set(CMAKE_CXX_STANDARD 14)

add_executable(NITCbase XFS_Interface_Code/disksimulator.cpp XFS_Interface_Code/interface_stack.cpp XFS_Interface_Code/block_access.hpp XFS_Interface_Code/algebra.hpp)