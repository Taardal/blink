cmake_minimum_required(VERSION 3.20.2)
set(CMAKE_CXX_STANDARD 17)

project(blink-cli)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif ()

add_executable(
        ${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/pch.cpp
        ${SRC_DIR}/pch.h
        ${SRC_DIR}/commands/commands.h
        ${SRC_DIR}/commands/generate.cpp
        ${SRC_DIR}/commands/build.cpp
        ${SRC_DIR}/commands/run.cpp
        ${SRC_DIR}/commands/compileLua.cpp
        ${SRC_DIR}/commands/compileShaders.cpp
)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_NAME bl
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}/debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}/release
)

get_target_property(BINARY_NAME ${PROJECT_NAME} RUNTIME_OUTPUT_NAME)
add_definitions(-DCMAKE_BINARY_NAME="${BINARY_NAME}")

# Set install command working directory to CLI project root (blink/cli/)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# Install binary to parent project (blink/)
install(
        FILES $<TARGET_FILE:${PROJECT_NAME}>
        DESTINATION ..
        RENAME ${BINARY_NAME}
)

#########################################
# Dependencies                          #
#########################################

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${SRC_DIR}/pch.h)

set(CLI_DIR ${LIB_DIR}/cli)
add_subdirectory(${CLI_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CLI_DIR}/include)
target_link_libraries(${PROJECT_NAME} cli)