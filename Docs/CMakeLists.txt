# Extract all public macros defined in LibCmo
# However, you should note that these extratcted macros have generator expressions.
get_target_property(LIBCMO_COMPILE_DEFINITIONS LibCmo COMPILE_DEFINITIONS)
if (LIBCMO_COMPILE_DEFINITIONS STREQUAL "LIBCMO_COMPILE_DEFINITIONS-NOTFOUND")
    message(FATAL_ERROR "Cannot extract compile definitions from LibCmo.")
endif ()
# Convert list to string for expanding in future.
list(JOIN LIBCMO_COMPILE_DEFINITIONS " " LIBCMO_MACRO_GENERATOR_EXPRESSIONS)

# We simply configure Doxygen config file first.
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)

# Then we use "file GENERATE" syntax to generate per-config truely Doxyfile used by Doxygen.
# Because there is no "$<>" syntax in Doxyfile, so we can safely use it.
# Please note that the generation of "file GENERATE" syntax will be postponed until the build stage.
file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Doxyfile"
    INPUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    TARGET LibCmo
)

# Add custom target using per-config Doxyfile
add_custom_target (LibCmoDocuments
    Doxygen::doxygen "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Doxyfile"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating documentation" VERBATIM
)

# Install built documentation
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION ${NEMO_INSTALL_DOC_PATH}
)
