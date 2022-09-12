# Run after all other rules within the target have been executed (PRE_BUILD & PRE_LINK).

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_DIR_NAME)
file(COPY ${RES_DIR} DESTINATION ${BIN_DIR}/${BUILD_TYPE_DIR_NAME})