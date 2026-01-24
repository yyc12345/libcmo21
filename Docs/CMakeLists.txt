# Configure Doxygen config file
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)

# Add custom target
add_custom_target (NeMoDocuments
    doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating LibCmo documentation" VERBATIM
)

# Install built documentation
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel
    DESTINATION ${NEMO_INSTALL_DOC_PATH}
)
