# Set the build directory
set(build_directory "${CMAKE_CURRENT_BINARY_DIR}/build")

# Run cmake --build
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${build_directory}
    RESULT_VARIABLE result
)

# Check the result variable for success or failure
if(result EQUAL 0)
    message("Build successful.")
else()
    message(FATAL_ERROR "Error building project.")
endif()
