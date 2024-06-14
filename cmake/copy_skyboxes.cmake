#######################################
# Pre-flight checks                   #
#######################################

if ("${SKYBOXES_SOURCE_DIR}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable SKYBOXES_SOURCE_DIR (model source code directory path)")
endif ()

if (NOT EXISTS ${SKYBOXES_SOURCE_DIR})
    message(FATAL_ERROR "Could not find model source code directory [${SKYBOXES_SOURCE_DIR}]")
endif ()

if ("${SKYBOXES_OUTPUT_DIR}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable SKYBOXES_OUTPUT_DIR (model compilation output directory path)")
endif ()

#######################################
# Copy skybox files                   #
#######################################

file(REMOVE_RECURSE ${SKYBOXES_OUTPUT_DIR})
file(MAKE_DIRECTORY ${SKYBOXES_OUTPUT_DIR})

file(GLOB SKYBOX_FILES "${SKYBOXES_SOURCE_DIR}/*")
foreach (SKYBOX_FILE ${SKYBOX_FILES})
    file(COPY ${SKYBOX_FILE} DESTINATION ${SKYBOXES_OUTPUT_DIR})
    message("Copied model file [${SKYBOX_FILE}] to [${SKYBOXES_OUTPUT_DIR}]")
endforeach ()
