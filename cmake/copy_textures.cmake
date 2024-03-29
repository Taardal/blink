#######################################
# Pre-flight checks                   #
#######################################

if ("${TEXTURES_SOURCE_DIR}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable TEXTURES_SOURCE_DIR (texture source code directory path)")
endif ()

if (NOT EXISTS ${TEXTURES_SOURCE_DIR})
    message(FATAL_ERROR "Could not find texture source code directory [${TEXTURES_SOURCE_DIR}]")
endif ()

if ("${TEXTURES_OUTPUT_DIR}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable TEXTURES_OUTPUT_DIR (texture compilation output directory path)")
endif ()

#######################################
# Copy texture files                  #
#######################################

file(REMOVE_RECURSE ${TEXTURES_OUTPUT_DIR})
file(MAKE_DIRECTORY ${TEXTURES_OUTPUT_DIR})

file(GLOB TEXTURE_FILES "${TEXTURES_SOURCE_DIR}/*")
foreach (TEXTURE_FILE ${TEXTURE_FILES})
    file(COPY ${TEXTURE_FILE} DESTINATION ${TEXTURES_OUTPUT_DIR})
    message("Copied texture file [${TEXTURE_FILE}] to [${TEXTURES_OUTPUT_DIR}]")
endforeach ()
