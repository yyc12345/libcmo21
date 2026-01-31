# Create shared library
add_library(BMap SHARED "")
# Setup sources
target_sources(BMap
PRIVATE
	BMap/BMap.cpp
	BMap/BMExports.cpp
)
# Setup headers
target_sources(BMap
PUBLIC
FILE_SET HEADERS
FILES
	BMap/BMap.hpp
	BMap/BMExports.hpp
)
# Setup header infomation
target_include_directories(BMap
PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
# Setup linked library infomation
target_link_libraries(BMap
PRIVATE
	YYCC::YYCCommonplace
	LibCmo
)
# Enable export macro
target_compile_definitions(BMap
PRIVATE
	BMAP_EXPORTING
)

# Install binary and headers
install(TARGETS BMap
    EXPORT BMapTargets
    RUNTIME DESTINATION ${NEMO_INSTALL_BIN_PATH}
    LIBRARY DESTINATION ${NEMO_INSTALL_BIN_PATH}
    ARCHIVE DESTINATION ${NEMO_INSTALL_LIB_PATH}
    INCLUDES DESTINATION ${NEMO_INSTALL_INCLUDE_PATH}
    FILE_SET HEADERS DESTINATION ${NEMO_INSTALL_INCLUDE_PATH}
)
# Install target
install(EXPORT BMapTargets
    FILE BMapTargets.cmake
    NAMESPACE NeMo::
    DESTINATION ${NEMO_INSTALL_LIB_PATH}/cmake/BMap
)
# Package configuration file
write_basic_package_version_file(
    BMapConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
)
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/../../CMake/BMapConfig.cmake.in 
    "${CMAKE_CURRENT_BINARY_DIR}/BMapConfig.cmake"
    INSTALL_DESTINATION ${NEMO_INSTALL_LIB_PATH}/cmake/BMap
)
# Copy package files to install destination
install(
FILES
    "${CMAKE_CURRENT_BINARY_DIR}/BMapConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/BMapConfigVersion.cmake"
DESTINATION
    ${NEMO_INSTALL_LIB_PATH}/cmake/BMap
)
