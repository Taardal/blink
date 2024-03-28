cmake_minimum_required(VERSION 3.17)
project(blink)

# Set C++ version
set(CMAKE_CXX_STANDARD 17)

# Generates a `compile_commands.json` file that contains compilation information for every source file in the project,
# facilitating better integration with various development tools and ensuring consistent and accurate builds across different environments.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure build type is always defined
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif ()
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_DIR_NAME)

# Project directory paths
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake)
set(DIST_DIR ${PROJECT_SOURCE_DIR}/dist)
set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/install)
set(LUA_DIR ${PROJECT_SOURCE_DIR}/lua)
set(RES_DIR ${PROJECT_SOURCE_DIR}/res)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

#########################################
# Main target                           #
#########################################

add_executable(
        ${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/pch.h
        ${SRC_DIR}/App.cpp
        ${SRC_DIR}/App.h
        ${SRC_DIR}/AppConfig.h
        ${SRC_DIR}/graphics/Quad.h
        ${SRC_DIR}/graphics/Renderer.cpp
        ${SRC_DIR}/graphics/Renderer.h
        ${SRC_DIR}/graphics/Texture.cpp
        ${SRC_DIR}/graphics/Texture.h
        ${SRC_DIR}/graphics/UniformBufferObject.h
        ${SRC_DIR}/graphics/Vertex.h
        ${SRC_DIR}/graphics/VertexAttribute.cpp
        ${SRC_DIR}/graphics/VertexAttribute.h
        ${SRC_DIR}/graphics/Vulkan.cpp
        ${SRC_DIR}/graphics/Vulkan.h
        ${SRC_DIR}/graphics/VulkanBuffer.cpp
        ${SRC_DIR}/graphics/VulkanBuffer.h
        ${SRC_DIR}/graphics/VulkanCommandPool.cpp
        ${SRC_DIR}/graphics/VulkanCommandPool.h
        ${SRC_DIR}/graphics/VulkanDevice.cpp
        ${SRC_DIR}/graphics/VulkanDevice.h
        ${SRC_DIR}/graphics/VulkanGraphicsPipeline.cpp
        ${SRC_DIR}/graphics/VulkanGraphicsPipeline.h
        ${SRC_DIR}/graphics/VulkanIndexBuffer.cpp
        ${SRC_DIR}/graphics/VulkanIndexBuffer.h
        ${SRC_DIR}/graphics/VulkanPhysicalDevice.cpp
        ${SRC_DIR}/graphics/VulkanPhysicalDevice.h
        ${SRC_DIR}/graphics/VulkanRenderPass.cpp
        ${SRC_DIR}/graphics/VulkanRenderPass.h
        ${SRC_DIR}/graphics/VulkanShader.cpp
        ${SRC_DIR}/graphics/VulkanShader.h
        ${SRC_DIR}/graphics/VulkanSwapChain.cpp
        ${SRC_DIR}/graphics/VulkanSwapChain.h
        ${SRC_DIR}/graphics/VulkanUniformBuffer.cpp
        ${SRC_DIR}/graphics/VulkanUniformBuffer.h
        ${SRC_DIR}/graphics/VulkanVertexBuffer.cpp
        ${SRC_DIR}/graphics/VulkanVertexBuffer.h
        ${SRC_DIR}/lua/EntityLuaBinding.cpp
        ${SRC_DIR}/lua/EntityLuaBinding.h
        ${SRC_DIR}/lua/KeyboardLuaBinding.cpp
        ${SRC_DIR}/lua/KeyboardLuaBinding.h
        ${SRC_DIR}/lua/LuaEngine.cpp
        ${SRC_DIR}/lua/LuaEngine.h
        ${SRC_DIR}/lua/luaUtils.cpp
        ${SRC_DIR}/lua/luaUtils.h
        ${SRC_DIR}/scene/Camera.cpp
        ${SRC_DIR}/scene/Camera.h
        ${SRC_DIR}/scene/Components.h
        ${SRC_DIR}/scene/Scene.cpp
        ${SRC_DIR}/scene/Scene.h
        ${SRC_DIR}/system/Assert.h
        ${SRC_DIR}/system/Environment.h
        ${SRC_DIR}/system/FileSystem.cpp
        ${SRC_DIR}/system/FileSystem.h
        ${SRC_DIR}/system/Log.cpp
        ${SRC_DIR}/system/Log.h
        ${SRC_DIR}/system/Random.cpp
        ${SRC_DIR}/system/Random.h
        ${SRC_DIR}/system/Uuid.cpp
        ${SRC_DIR}/system/Uuid.h
        ${SRC_DIR}/utils/size.h
        ${SRC_DIR}/window/Event.cpp
        ${SRC_DIR}/window/Event.h
        ${SRC_DIR}/window/Keyboard.cpp
        ${SRC_DIR}/window/Keyboard.h
        ${SRC_DIR}/window/KeyEvent.cpp
        ${SRC_DIR}/window/KeyEvent.h
        ${SRC_DIR}/window/MouseEvent.cpp
        ${SRC_DIR}/window/MouseEvent.h
        ${SRC_DIR}/window/Window.cpp
        ${SRC_DIR}/window/Window.h
        ${SRC_DIR}/window/WindowEvent.cpp
        ${SRC_DIR}/window/WindowEvent.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${SRC_DIR}/pch.h)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_NAME ${PROJECT_NAME}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}/debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}/release
)

#########################################
# Custom targets                        #
#########################################

set(LUA_SOURCE_DIR ${PROJECT_SOURCE_DIR}/lua)
set(LUA_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin/${BUILD_TYPE_DIR_NAME}/lua)
add_custom_target(
        CompileLua
        COMMAND ${CMAKE_COMMAND} -D LUA_SOURCE_DIR=${LUA_SOURCE_DIR} -D LUA_OUTPUT_DIR=${LUA_OUTPUT_DIR} -P ${CMAKE_DIR}/compile_lua.cmake
        COMMENT "Compiling Lua"
)

set(SHADERS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/res/shaders)
set(SHADERS_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin/${BUILD_TYPE_DIR_NAME}/shaders)
add_custom_target(
        CompileShaders
        COMMAND ${CMAKE_COMMAND} -D SHADERS_SOURCE_DIR=${SHADERS_SOURCE_DIR} -D SHADERS_OUTPUT_DIR=${SHADERS_OUTPUT_DIR} -P ${CMAKE_DIR}/compile_shaders.cmake
        COMMENT "Compiling shaders"
)

add_dependencies(${PROJECT_NAME} CompileLua CompileShaders)

#########################################
# Preprocessor macros (used in C++)     #
#########################################

add_definitions(-DCMAKE_SCRIPTS_DIR="${CMAKE_DIR}")
add_definitions(-DCMAKE_LUA_SOURCE_DIR="${LUA_SOURCE_DIR}")
add_definitions(-DCMAKE_LUA_OUTPUT_DIR="${LUA_OUTPUT_DIR}")
add_definitions(-DCMAKE_SHADERS_SOURCE_DIR="${SHADERS_SOURCE_DIR}")
add_definitions(-DCMAKE_SHADERS_OUTPUT_DIR="${SHADERS_OUTPUT_DIR}")

#########################################
# Dependencies                          #
#########################################

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

include(FetchContent)
message("-- Fetching dependencies")
FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt
        GIT_TAG v3.13.1
)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.4
)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 1.0.1
)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.9.2
)
message("-- Fetching dependencies - done")
message("-- Compiling dependencies")
FetchContent_MakeAvailable(
        entt
        glfw
        glm
        spdlog
)
message("-- Compiling dependencies - done")

target_link_libraries(${PROJECT_NAME} EnTT::EnTT)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)

find_package(Lua REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})

#########################################
# Installation                          #
#########################################

# Output directory for the installed targets
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})

if (APPLE)
    # Set RPATH
    # - https://en.wikipedia.org/wiki/Rpath
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "@executable_path/lib")
endif ()

# Install executable
install(TARGETS ${PROJECT_NAME} DESTINATION .)

# Install resources
install(DIRECTORY ${LUA_OUTPUT_DIR} DESTINATION .)
install(DIRECTORY ${SHADERS_OUTPUT_DIR} DESTINATION .)

# Install Lua library (dynamically linked)
get_filename_component(LUA_LIBRARY_PATH "${LUA_LIBRARIES}" REALPATH)
install(FILES ${LUA_LIBRARY_PATH} DESTINATION lib)

# Install Vulkan library (dynamically linked)
get_filename_component(VULKAN_LIBRARY_PATH "${Vulkan_LIBRARIES}" REALPATH)
install(FILES ${VULKAN_LIBRARY_PATH} DESTINATION lib)
if (APPLE)
    # Install Vulkan library symlinks
    # - https://vulkan.lunarg.com/doc/sdk/1.3.275.0/mac/getting_started.html
    #   - Section "Using the Vulkan Loader"
    get_filename_component(VULKAN_LIBRARY_NAME "${Vulkan_LIBRARIES}" NAME)
    string(REPLACE ".dylib" "" VULKAN_LIBRARY_NAME "${VULKAN_LIBRARY_NAME}")
    install(CODE "file(CREATE_LINK \"${VULKAN_LIBRARY_PATH}\" \"${CMAKE_INSTALL_PREFIX}/lib/${VULKAN_LIBRARY_NAME}.dylib\")")
    install(CODE "file(CREATE_LINK \"${VULKAN_LIBRARY_PATH}\" \"${CMAKE_INSTALL_PREFIX}/lib/${VULKAN_LIBRARY_NAME}.1.dylib\")")
endif ()

#########################################
# Packaging                             #
#########################################

# Output directory for the generated package
set(CPACK_OUTPUT_FILE_PREFIX ${DIST_DIR})

set(CPACK_PACKAGE_NAME "Blink")
set(CPACK_PACKAGE_VENDOR "Storytime")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Proof-of-concept of a game featuring a Vulkan rendering and Lua scripts")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_GENERATOR "ZIP")
set(CPACK_SOURCE_GENERATOR "ZIP")

# Regarding quadruple slashes (\\\\):
# To use a single backslash in a regular expression pattern within CMake, you need to escape it with another backslash
# However, regular expressions within CMake requires escaping backslashes so you need to escape the escape character itself.
set(
        CPACK_SOURCE_IGNORE_FILES
        "/.cache/"      # clangd
        "/.DS_Store"    # macOS
        "/.git/"        # Git
        "/.idea/"       # Jetbrains CLion IDE
        "/bin/"         # CMake build output
        "/build/"       # CMake build files
        "/run.sh"       # Run-script
        "/\\\\.swp$"    # Vim swap files
)

include(CPack)
