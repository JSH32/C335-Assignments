cmake_minimum_required(VERSION 3.5.0)
project(C335Assignments VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_FLAGS "-Wall -Werror -std=c11 -pedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ASSIGNMENTS_DIR "${CMAKE_SOURCE_DIR}/assignments")
file(GLOB ASSIGNMENT_DIRS RELATIVE ${ASSIGNMENTS_DIR} ${ASSIGNMENTS_DIR}/*)

# Function to check if a source file contains a 'main' function
function(contains_main_function result filepath)
    file(READ "${filepath}" content)
    string(REGEX MATCH "int[ \t]+main[ \t]*\\(" has_main "${content}")
    if(has_main)
        set(${result} TRUE PARENT_SCOPE)
    else()
        set(${result} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Iterate over each subdirectory and create build targets for files with 'main' functions
foreach(ASSIGNMENT_DIR ${ASSIGNMENT_DIRS})
    if(IS_DIRECTORY ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR})
        # Collect all source files recursively within the assignment directory
        file(GLOB_RECURSE ASSIGNMENT_SOURCES
            ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR}/*.c
            ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR}/*.h
        )

        set(main_sources "")  # List of sources containing 'main'
        set(non_main_sources "") # List of sources without 'main'

        # Check for files containing 'main' and add them to main_sources
        foreach(source_file ${ASSIGNMENT_SOURCES})
            get_filename_component(source_name ${source_file} NAME_WE)
            contains_main_function(has_main "${source_file}")
            if(has_main)
                # Add file with 'main' to main_sources
                list(APPEND main_sources ${source_file})
            else()
                # Add non-main files to non_main_sources
                list(APPEND non_main_sources ${source_file})
            endif()
        endforeach()

        # Create a separate target for each file with 'main'
        foreach(main_source ${main_sources})
            get_filename_component(main_source_name ${main_source} NAME_WE)
            set(target_name "${ASSIGNMENT_DIR}_${main_source_name}")

            # Create target with the main source and other non-main sources
            add_executable(${target_name} ${main_source} ${non_main_sources})

            target_include_directories(${target_name} PRIVATE ${ASSIGNMENTS_DIR}/${ASSIGNMENT_DIR})
            message(STATUS "Created target: ${target_name} with main source ${main_source}")
        endforeach()
    endif()
endforeach()
