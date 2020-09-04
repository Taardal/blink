cmake_minimum_required(VERSION 3.17)
project(blink)

set(CMAKE_CXX_STANDARD 20)

add_executable(blink src/main.cpp src/LuaClient.cpp src/LuaClient.h)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(LUA_NAME lua)
set(LUA_DIR lib/lua-5.4.0)
add_subdirectory(${LUA_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_DIR}/include)
target_link_libraries(${PROJECT_NAME} ${LUA_NAME})