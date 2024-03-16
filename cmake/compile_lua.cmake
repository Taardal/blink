#######################################
# Pre-flight checks                   #
#######################################

if ("${LUA_SOURCE_DIR}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable LUA_SOURCE_DIR (Lua source code directory path)")
endif ()

if (NOT EXISTS ${LUA_SOURCE_DIR})
    message(FATAL_ERROR "Could not find Lua source code directory [${LUA_SOURCE_DIR}]")
endif ()

if ("${LUA_OUTPUT_DIR}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable LUA_OUTPUT_DIR (Lua compilation output directory path)")
endif ()

find_program(LUAC luac)
if (NOT LUAC)
    message(FATAL_ERROR "Could not find Lua compiler (luac)")
endif ()

#######################################
# Compile lua files                   #
#######################################

file(REMOVE_RECURSE ${LUA_OUTPUT_DIR})
file(MAKE_DIRECTORY "${LUA_OUTPUT_DIR}")

file(GLOB LUA_FILES "${LUA_SOURCE_DIR}/*.lua")
foreach (LUA_FILE ${LUA_FILES})
    get_filename_component(FILENAME ${LUA_FILE} NAME_WE)
    set(OUTPUT_FILE "${LUA_OUTPUT_DIR}/${FILENAME}.out")
    execute_process(
            COMMAND ${LUAC} -o ${OUTPUT_FILE} ${LUA_FILE}
            RESULT_VARIABLE result
    )
    if (result EQUAL 0)
        message("Compiled Lua file [${LUA_FILE}] to [${OUTPUT_FILE}]")
    else ()
        message(FATAL_ERROR "Could not compile Lua file [${LUA_FILE}]")
    endif ()
endforeach ()

