cmake_minimum_required(VERSION 3.5.0)
project(C335Assignments VERSION 0.1.0 LANGUAGES C)

# set(CMAKE_CXX_FLAGS "-Wall -Werror -std=c11 -pedantic")

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# add_executable(rpn rpn_calculator.c)

set(ASSIGNMENTS_DIR "${CMAKE_SOURCE_DIR}/assignments")

# Get the list of all subdirectories in the assignments folder
file(GLOB ASSIGNMENT_DIRS RELATIVE ${ASSIGNMENTS_DIR} ${ASSIGNMENTS_DIR}/*)

# Iterate over each subdirectory and create a build target
foreach(ASSIGNMENT_DIR ${ASSIGNMENT_DIRS})
    if(IS_DIRECTORY ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR})
        # Collect all source and header files recursively within the assignment directory
        file(GLOB_RECURSE ASSIGNMENT_SOURCES
            # ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR}/*.cpp
            ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR}/*.c
            ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR}/*.h
            # ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR}/*.hpp
        )

        # Create a target
        add_executable(${ASSIGNMENT_DIR} ${ASSIGNMENT_SOURCES})

        # Include directories
        target_include_directories(${ASSIGNMENT_DIR} PRIVATE ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR})
    endif()
endforeach()