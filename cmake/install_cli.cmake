message("
#######################################
#  Setting up...                      #
#######################################
")

option(RELEASE "Use Release as the CMake build type" OFF)
option(NINJA "Use Ninja as the CMake generator" OFF)

# CMake build type (default: Debug)
if (RELEASE)
    set(CMAKE_BUILD_TYPE "Release")
else ()
    set(CMAKE_BUILD_TYPE "Debug")
endif ()
message("-- Using CMake build type [${CMAKE_BUILD_TYPE}]")

# CMake generator (default: System detect)
if (NINJA)
    set(GENERATOR "Ninja")
endif ()
if (GENERATOR)
    message("-- Using CMake generator [${GENERATOR}]")
endif ()

# Work from the CLI directory (`<project root>/cli`)
if ("${CMAKE_CURRENT_SOURCE_DIR}" MATCHES "/cmake$")
    set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cli)
else ()
    set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cli)
endif ()
message("-- Working from directory [${CMAKE_CURRENT_SOURCE_DIR}]")

set(SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
message("-- Using CMake source directory [${SOURCE_DIRECTORY}]")

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_DIR_NAME)
set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/${BUILD_TYPE_DIR_NAME}")
message("-- Using CMake build directory [${BUILD_DIRECTORY}]")

message("
#######################################
#  Generating build files...          #
#######################################
")

if (GENERATOR)
    set(GENERATOR_ARG "-G ${GENERATOR}")
endif ()
execute_process(
        COMMAND ${CMAKE_COMMAND}
        -D CMAKE_EXPORT_COMPILE_COMMANDS=1
        -D CMAKE_BUILD_TYPE=${buildType}
        -B ${BUILD_DIRECTORY}
        -S ${SOURCE_DIRECTORY}
        ${GENERATOR_ARG}
)

message("
#######################################
#  Building binary...                 #
#######################################
")

execute_process(COMMAND cmake --build "${BUILD_DIRECTORY}" --config "${CMAKE_BUILD_TYPE}")

message("
#######################################
#  Installing binary...               #
#######################################
")

execute_process(COMMAND cmake --install "${BUILD_DIRECTORY}" --config "${CMAKE_BUILD_TYPE}")
