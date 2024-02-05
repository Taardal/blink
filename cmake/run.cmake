message("Hello CMAKE")

message("run")
message("Command: ${CMAKE_COMMAND}")

# Set the path to the binary file
set(binary_directory_path "${CMAKE_CURRENT_BINARY_DIR}/bin/debug")
set(binary_path "${binary_directory_path}/blink")

# Run the binary
execute_process(
    COMMAND ${binary_path}
    WORKING_DIRECTORY ${binary_directory_path}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)

# Print the output
message("Output from blink:")
message("${output}")

# Check the result variable for success or failure
if(result EQUAL 0)
    message("blink executed successfully.")
else()
    message(FATAL_ERROR "Error running blink: ${error_output}")
endif()

