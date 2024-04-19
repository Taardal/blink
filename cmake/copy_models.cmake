#######################################
# Pre-flight checks                   #
#######################################

if ("${MODELS_SOURCE_DIR}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable MODELS_SOURCE_DIR (model source code directory path)")
endif ()

if (NOT EXISTS ${MODELS_SOURCE_DIR})
    message(FATAL_ERROR "Could not find model source code directory [${MODELS_SOURCE_DIR}]")
endif ()

if ("${MODELS_OUTPUT_DIR}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable MODELS_OUTPUT_DIR (model compilation output directory path)")
endif ()

#######################################
# Copy model files                    #
#######################################

file(REMOVE_RECURSE ${MODELS_OUTPUT_DIR})
file(MAKE_DIRECTORY ${MODELS_OUTPUT_DIR})

file(GLOB MODEL_FILES "${MODELS_SOURCE_DIR}/*")
foreach (MODEL_FILE ${MODEL_FILES})
    file(COPY ${MODEL_FILE} DESTINATION ${MODELS_OUTPUT_DIR})
    message("Copied model file [${MODEL_FILE}] to [${MODELS_OUTPUT_DIR}]")
endforeach ()
