message("
#######################################
#  Setting up...                      #
#######################################
")

option(GENERATOR "Which CMake generator to use" "")

if (GENERATOR)
    message("-- Using CMake generator [${GENERATOR}]")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif ()
message("-- Using CMake build type [${CMAKE_BUILD_TYPE}]")

if ("${CMAKE_CURRENT_SOURCE_DIR}" MATCHES "/cmake$")
    set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
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
        -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -B ${BUILD_DIRECTORY}
        -S ${SOURCE_DIRECTORY}
        ${GENERATOR_ARG}
)