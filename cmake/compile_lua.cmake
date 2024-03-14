message("Lua directory: ${LUA_DIR}")
if(NOT EXISTS ${LUA_DIR})
    message(FATAL_ERROR "Lua directory not found: ${LUA_DIR}")
endif()

find_program(LUAC luac)
if(NOT LUAC)
    message(FATAL_ERROR "Lua compiler (luac) not found")
endif()

file(GLOB lua_files "${LUA_DIR}/*.lua")
foreach(lua_file ${lua_files})
    get_filename_component(output_file ${lua_file} NAME_WE)
    set(output_file "${OUTPUT_DIR}/${output_file}.out")
    execute_process(
            COMMAND ${LUAC} -o ${output_file} ${lua_file}
            RESULT_VARIABLE result
    )
    if(result EQUAL 0)
        message("Compiled: ${lua_file}")
    else()
        message("Compilation failed for: ${lua_file}")
    endif()
endforeach()