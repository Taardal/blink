cmake_minimum_required(VERSION 3.17)
project(blink)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        blink
        src/main.cpp
        src/Application.cpp
        src/Application.h
        src/Config.h
        src/Engine.cpp
        src/Engine.h
        src/GraphicsContext.cpp
        src/GraphicsContext.h
        src/FileSystem.cpp
        src/FileSystem.h
        src/Log.cpp
        src/Log.h
        src/LuaBinding.cpp
        src/LuaBinding.h
        src/LuaBindingListener.h
        src/LuaClient.cpp
        src/LuaClient.h
        src/LuaGraphicsBinding.cpp
        src/LuaGraphicsBinding.h
        src/LuaPrintStack.cpp
        src/LuaPrintStack.h
        src/Renderer.cpp
        src/Renderer.h
        src/Tag.cpp
        src/Tag.h
        src/Window.cpp
        src/Window.h
)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(RES_DIR ${PROJECT_SOURCE_DIR}/res)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(GLAD_NAME glad)
set(GLAD_DIR ${LIB_DIR}/glad-0.1.33)
add_library(${GLAD_NAME} ${GLAD_DIR}/src/glad.c)
target_include_directories(${GLAD_NAME} PRIVATE ${GLAD_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLAD_DIR}/include)
target_link_libraries(${PROJECT_NAME} glad ${CMAKE_DL_LIBS})

set(GLFW_DIR ${LIB_DIR}/glfw-3.3)
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL Build the GLFW example programs)
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL Build the GLFW test programs)
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL Build the GLFW documentation)
set(GLFW_INSTALL OFF CACHE INTERNAL Generate installation target)
add_subdirectory(${GLFW_DIR})
target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_NONE)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_DIR}/include)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})

set(GLM_DIR ${LIB_DIR}/glm-0.9.9.6)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_DIR})

set(LUA_NAME lua)
set(LUA_DIR lib/lua-5.4.0)
add_subdirectory(${LUA_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_DIR}/include)
target_link_libraries(${PROJECT_NAME} ${LUA_NAME})

set(SPDLOG_DIR ${LIB_DIR}/spdlog-1.4.2)
target_include_directories(${PROJECT_NAME} PUBLIC ${SPDLOG_DIR}/include)

if (WIN32)
    add_compile_definitions(ST_PLATFORM_WINDOWS)
    set(PLATFORM_FOUND TRUE)
    if (MSVC)
        add_compile_definitions(ST_COMPILER_MSVC)
        set(COMPILER_FOUND TRUE)
    endif()
endif()

if (UNIX AND APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_compile_definitions(ST_PLATFORM_MACOS)
    set(PLATFORM_FOUND TRUE)
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        add_compile_definitions(ST_COMPILER_CLANG)
        set(COMPILER_FOUND TRUE)
    endif()
endif()

if (NOT PLATFORM_FOUND)
    message(FATAL_ERROR "-- Could not find platform [${CMAKE_SYSTEM_NAME}]")
endif()

if (NOT COMPILER_FOUND)
    message(FATAL_ERROR "-- Could not find compiler [${CMAKE_CXX_COMPILER_ID}]")
endif()