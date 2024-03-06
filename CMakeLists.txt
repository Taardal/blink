cmake_minimum_required(VERSION 3.17)
project(blink)

set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(RES_DIR ${PROJECT_SOURCE_DIR}/res)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif ()

add_executable(
        ${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/pch.h
        ${SRC_DIR}/App.cpp
        ${SRC_DIR}/App.h
        ${SRC_DIR}/AppConfig.h
#        ${SRC_DIR}/LuaClient.cpp
#        ${SRC_DIR}/LuaClient.h
#        ${SRC_DIR}/LuaEngine.cpp
#        ${SRC_DIR}/LuaEngine.h
#        ${SRC_DIR}/LuaEngineListener.h
#        ${SRC_DIR}/LuaGraphicsEngine.cpp
#        ${SRC_DIR}/LuaGraphicsEngine.h
        ${SRC_DIR}/game/Camera.cpp
        ${SRC_DIR}/game/Camera.h
        ${SRC_DIR}/game/Components.h
        ${SRC_DIR}/game/Game.cpp
        ${SRC_DIR}/game/Game.h
        ${SRC_DIR}/game/GameModule.cpp
        ${SRC_DIR}/game/GameModule.h
        ${SRC_DIR}/game/Scene.cpp
        ${SRC_DIR}/game/Scene.h
        ${SRC_DIR}/graphics/GraphicsModule.cpp
        ${SRC_DIR}/graphics/GraphicsModule.h
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
        ${SRC_DIR}/lua/LuaEngine.cpp
        ${SRC_DIR}/lua/LuaEngine.h
        ${SRC_DIR}/lua/LuaFunction.cpp
        ${SRC_DIR}/lua/LuaFunction.h
        ${SRC_DIR}/lua/LuaModule.cpp
        ${SRC_DIR}/lua/LuaModule.h
        ${SRC_DIR}/lua/printLua.h
        ${SRC_DIR}/system/Assert.h
        ${SRC_DIR}/system/Environment.h
        ${SRC_DIR}/system/FileSystem.cpp
        ${SRC_DIR}/system/FileSystem.h
        ${SRC_DIR}/system/Log.cpp
        ${SRC_DIR}/system/Log.h
        ${SRC_DIR}/system/Random.cpp
        ${SRC_DIR}/system/Random.h
        ${SRC_DIR}/system/SystemModule.cpp
        ${SRC_DIR}/system/SystemModule.h
        ${SRC_DIR}/system/Uuid.cpp
        ${SRC_DIR}/system/Uuid.h
        ${SRC_DIR}/utils/size.h
        ${SRC_DIR}/window/Keyboard.cpp
        ${SRC_DIR}/window/Keyboard.h
        ${SRC_DIR}/window/Window.cpp
        ${SRC_DIR}/window/Window.h
        ${SRC_DIR}/window/WindowModule.cpp
        ${SRC_DIR}/window/WindowModule.h
)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_NAME ${PROJECT_NAME}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}/debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}/release
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${SRC_DIR}/pch.h)

set(ENTT_DIR ${LIB_DIR}/entt)
add_subdirectory(${ENTT_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${ENTT_DIR})
target_link_libraries(${PROJECT_NAME} EnTT::EnTT)

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

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

