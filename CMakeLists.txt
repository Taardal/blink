cmake_minimum_required(VERSION 3.17)
project(blink)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        blink
        src/main.cpp
        src/LuaClient.cpp
        src/LuaClient.h
        src/FileSystem.cpp
        src/FileSystem.h
        src/Engine.cpp
        src/Engine.h
        src/LuaEngine.cpp
        src/LuaEngine.h
        src/Config.h
        src/Application.cpp
        src/Application.h
        src/Window.cpp
        src/Window.h
        src/GraphicsContext.cpp
        src/GraphicsContext.h
        src/Log.cpp
        src/Log.h
        src/Tag.cpp
        src/Tag.h
        src/LuaGraphicsEngine.cpp
        src/LuaGraphicsEngine.h
        src/Renderer.cpp
        src/Renderer.h
        src/LuaEngineListener.h
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

set(SPD_LOG_DIR ${LIB_DIR}/spdlog-1.4.2)
target_include_directories(${PROJECT_NAME} PUBLIC ${SPD_LOG_DIR}/include)