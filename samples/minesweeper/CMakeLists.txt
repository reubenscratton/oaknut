cmake_minimum_required(VERSION 3.9)
project(Minesweeper)

if (NOT IS_DIRECTORY $ENV{OAKNUT_DIR})
    message(FATAL_ERROR "Environment variable OAKNUT_DIR must point to the Oaknut repo")
endif()

include($ENV{OAKNUT_DIR}/CMakeLists.txt)

