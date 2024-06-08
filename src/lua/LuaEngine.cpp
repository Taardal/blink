#include "pch.h"
#include "lua/LuaEngine.h"
#include "lua/luaUtils.h"
#include "lua/CoordinateSystemLuaBinding.h"
#include "lua/EntityLuaBinding.h"
#include "lua/GlmLuaBinding.h"
#include "lua/KeyboardLuaBinding.h"
#include "lua/SceneCameraLuaBinding.h"
#include "lua/WindowLuaBinding.h"
#include "scene/Components.h"
#include "scene/Scene.h"

#include <lua.hpp>

namespace Blink {
    LuaEngine::LuaEngine(const LuaEngineConfig& config) : config(config) {
        initialize();
    }

    LuaEngine::~LuaEngine() {
        terminate();
    }

    void LuaEngine::resetState() {
        terminate();
        initialize();
    }

    void LuaEngine::initializeCoreBindings(Scene* scene) const {
        CoordinateSystemLuaBinding::initialize(L);
        EntityLuaBinding::initialize(L, scene);
        GlmLuaBinding::initialize(L);
        KeyboardLuaBinding::initialize(L, config.keyboard);
        SceneCameraLuaBinding::initialize(L, config.sceneCamera);
        WindowLuaBinding::initialize(L, config.window);
    }

    void LuaEngine::initializeEntityBinding(entt::entity entity, const LuaComponent& luaComponent, const TagComponent& tagComponent) const {
        const std::string& tableName = luaComponent.type;
        const std::string& filepath = luaComponent.path;

        lua_newtable(L);
        lua_setglobal(L, tableName.c_str());

        if (luaL_dofile(L, filepath.c_str()) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR(
                "Could not load Lua script [{}] for entity [id: {}, type: {}, tag: {}]: {}",
                filepath,
                tableName,
                tagComponent.tag,
                errorMessage
            );
            BL_THROW("Could not initialize entity binding");
        }
        BL_LOG_INFO(
            "Loaded Lua script [{}] for entity [id: {}, type: {}, tag: {}]",
            filepath,
            entity,
            tableName,
            tagComponent.tag
        );
    }

    void LuaEngine::configureSceneCamera(const std::string& sceneFilePath) const {
        const char* tableName = "Scene";
        const char* functionName = "onConfigureCamera";

        lua_newtable(L);
        lua_setglobal(L, tableName);

        if (luaL_dofile(L, sceneFilePath.c_str()) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR("Could not load Lua script [{}]: {}", sceneFilePath, errorMessage);
            BL_THROW("Could not load Lua script");
        }
        BL_LOG_INFO("Loaded Lua script [{}]", sceneFilePath);

        lua_pushcfunction(L, printLuaError);
        lua_getglobal(L, tableName);
        lua_getfield(L, -1, functionName);

        bool functionMissing = lua_isnil(L, -1);
        if (!functionMissing) {
            constexpr int argumentCount = 0;
            constexpr int returnValueCount = 0;
            constexpr int errorHandlerIndex = -3;
            if (lua_pcall(L, argumentCount, returnValueCount, errorHandlerIndex) != LUA_OK) {
                const char* errorMessage = lua_tostring(L, -1);
                BL_LOG_ERROR(
                    "Could not invoke [{}:{}:{}]: {}",
                    sceneFilePath,
                    tableName,
                    functionName,
                    errorMessage
                );
                BL_THROW("Could not configure scene camera");
            }
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaEngine::createEntities(const std::string& sceneFilePath) const {
        const char* tableName = "Scene";
        const char* functionName = "onCreateEntities";

        lua_newtable(L);
        lua_setglobal(L, tableName);
        if (luaL_dofile(L, sceneFilePath.c_str()) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR(
                "Could not load Lua script [{}]: {}",
                sceneFilePath,
                errorMessage
            );
            BL_THROW("Could not load Lua script");
        }
        BL_LOG_INFO(
            "Loaded Lua script [{}]",
            sceneFilePath
        );

        lua_pushcfunction(L, printLuaError);
        lua_getglobal(L, tableName);
        lua_getfield(L, -1, functionName);

        constexpr int argumentCount = 0;
        constexpr int returnValueCount = 0;
        constexpr int errorHandlerIndex = -3;

        if (lua_pcall(L, argumentCount, returnValueCount, errorHandlerIndex) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR(
                "Could not invoke [{}:{}:{}]: {}",
                sceneFilePath,
                tableName,
                functionName,
                errorMessage
            );
            BL_THROW("Could not call Lua function");
        }
        lua_pop(L, lua_gettop(L));
    }

    void LuaEngine::updateEntity(entt::entity entity, const LuaComponent& luaComponent, const TagComponent& tagComponent, double timestep) const {
        static const char* functionName = "onUpdate";
        std::string tableName = luaComponent.type;

        lua_pushcfunction(L, printLuaError);
        lua_getglobal(L, tableName.c_str());
        lua_getfield(L, -1, functionName);
        lua_pushnumber(L, (uint32_t) entity);
        lua_pushnumber(L, timestep);

        constexpr int argumentCount = 2;
        constexpr int returnValueCount = 0;
        constexpr int errorHandlerIndex = -5;

        if (lua_pcall(L, argumentCount, returnValueCount, errorHandlerIndex) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR(
                "Could not invoke [{}:{}:{}] for entity [id: {}, tag: {}]: {}",
                luaComponent.path,
                luaComponent.type,
                functionName,
                entity,
                tagComponent.tag,
                errorMessage
            );
            BL_THROW("Could not update entity");
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaEngine::compileLuaFiles() const {
        std::stringstream ss;
        ss << "cmake";
        ss << " -D LUA_SOURCE_DIR=" << CMAKE_LUA_SOURCE_DIR;
        ss << " -D LUA_OUTPUT_DIR=" << CMAKE_LUA_OUTPUT_DIR;
        ss << " -P " << CMAKE_SCRIPTS_DIR << "/compile_lua.cmake";
        std::string command = ss.str();
        std::system(command.c_str());
    }

    void LuaEngine::initialize() {
        L = luaL_newstate();

        // Enable Lua standard libraries
        luaL_openlibs(L);

        // Add paths to be searched for lua files
        luaL_dostring(L, "package.path = './lua/?.out;' .. package.path");

        // Override Lua 'print' function with custom logger
        lua_pushcfunction(L, LuaEngine::printLuaMessage);
        lua_setglobal(L, "print");
    }

    void LuaEngine::terminate() const {
        lua_close(L);
    }

    int LuaEngine::printLuaMessage(lua_State* L) {
        const char* msg = lua_tostring(L, -1);
        BL_LOG_INFO("[LUA] - {}", msg);
        return 0;
    }

    int LuaEngine::printLuaError(lua_State* L) {
        std::cerr << "--------------------------------------------------------------------------------------------------------------" << std::endl;
        std::cerr << "[Lua error] " << lua_tostring(L, -1) << std::endl;
        std::cerr << "--------------------------------------------------------------------------------------------------------------" << std::endl;

        lua_Debug debugInfo;
        uint32_t stackLevel = 0;

        // Populate lua_Debug with information about the interpreter runtime stack
        // Level 0 is the current running function, whereas level n+1 is the function that has called level n (except for tail calls, which do not count in the stack).
        // When called with a level greater than the stack depth, lua_getstack returns 0; otherwise it returns 1.
        while (lua_getstack(L, stackLevel, &debugInfo) > 0) {

            // Get information about a specific function or function invocation.
            // - [n] fills in the field name and namewhat;
            // - [S] fills in the fields source, short_src, linedefined, lastlinedefined, and what;
            // - [l] fills in the field currentline;
            // - [t] fills in the field istailcall;
            // - [u] fills in the fields nups, nparams, and isvararg;
            // - [f] pushes onto the stack the function that is running at the given level;
            // - [L] pushes onto the stack a table whose indices are the numbers of the lines that are valid on the function.
            //      - A valid line is a line with some associated code, that is, a line where you can put a break point.
            //      - Non-valid lines include empty lines and comments.
            lua_getinfo(L, "nSltuf", &debugInfo);

            // A reasonable name for the given function.
            // Because functions in Lua are first-class values, they do not have a fixed name.
            // The lua_getinfo function checks how the function was called to find a suitable name.
            // If it cannot find a name, then name is set to NULL.
            std::string name = debugInfo.name ? debugInfo.name : "";

            // Explains the name field according to how the function was called.
            // Possible values:
            // - [global]
            // - [local]
            // - [method]
            // - [field]
            // - [upvalue]
            // - [""] (Empty string when no other option seems to apply)
            std::string namewhat = debugInfo.namewhat != nullptr ? debugInfo.namewhat : "";

            // What type of function it is
            // - [Lua] if the function is a Lua function
            // - [C] if it is a C function
            // - [main] if it is the main part of a chunk
            std::string what = debugInfo.what != nullptr ? debugInfo.what : "";

            // The source of the chunk that created the function.
            // If the source starts with...
            // - [@] The function was defined in a file where the file name follows the '@'
            // - [=] The remainder of its contents describes the source in a user-dependent manner.
            // - [ ] The function was defined in a string where source is that string.
            std::string source = debugInfo.source != nullptr ? debugInfo.source : "";

            // A "printable" version of source, to be used in error messages.
            std::string shortSource = strlen(debugInfo.short_src) > 0 ? debugInfo.short_src : "";

            // The current line where the given function is executing. When no line information is available, currentline is set to -1.
            int currentLine = debugInfo.currentline;

            // The line number where the definition of the function starts.
            int lineDefined = debugInfo.linedefined;

            // The line number where the definition of the function ends.
            int lastLineDefined = debugInfo.lastlinedefined;

            // The number of upvalues of the function.
            int numberOfUpvalues = debugInfo.nups;

            // The number of parameters of the function (always 0 for C functions).
            int numberOfParameters = debugInfo.nparams;

            // True if the function is a variadic function (always true for C functions).
            bool variadicFunction = debugInfo.isvararg;

            // True if this function invocation was called by a tail call.
            // In this case, the caller of this level is not in the stack.
            bool tailCall = debugInfo.istailcall;

            std::string filename = "";
            size_t sourceLastSlashIndex = source.find_last_of("/");
            if (sourceLastSlashIndex != std::string::npos && sourceLastSlashIndex + 1 < source.length()) {
                filename = source.substr(sourceLastSlashIndex + 1);
            }

            std::stringstream ss;
            if (filename.length() > 0) {
                ss << filename;
            }
            if (currentLine > -1) {
                if (filename.length() > 0) {
                    ss << ":";
                }
                ss << currentLine;
            }
            std::string tag = ss.str();
            std::cerr << "[" << stackLevel << "]" << " " << tag << std::endl;

            std::cerr << "  Source: " << (source.length() > 0 ? source : "--") << std::endl;
            std::cerr << "  Line: " << currentLine << std::endl;
            std::cerr << "  Name: " << (name.length() > 0 ? name : "--") << std::endl;
            std::cerr << "  Description: " << (namewhat.length() > 0 ? namewhat : "--") << std::endl;
            std::cerr << "  Parameters: " << numberOfParameters << std::endl;
            std::cerr << "  Upvalues: " << numberOfUpvalues << std::endl;
            std::cerr << "  Start line: " << lineDefined << std::endl;
            std::cerr << "  End line: " << lastLineDefined << std::endl;
            std::cerr << "  Variadic function: " << (variadicFunction ? "Yes" : "No") << std::endl;
            std::cerr << "  Tail call: " << (tailCall ? "Yes" : "No") << std::endl;

            lua_pop(L, 1); // Remove the function pushed by lua_getstack
            ++stackLevel;
        }
        std::cerr << "--------------------------------------------------------------------------------------------------------------" << std::endl;
        std::cerr << std::endl;

        return 1; // Return the error message
    }
}
