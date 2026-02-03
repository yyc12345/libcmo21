# - Find STB library
# Find the STB headers
#
# This module defines the following variables:
#   STB_FOUND        - True if STB was found
#   STB_INCLUDE_DIRS - Location of the STB headers
#
# This module defines the following imported targets:
#   STB::STB - Header-only interface library for STB

# STB_ROOT must be specified by the user
if (NOT DEFINED STB_ROOT)
    set(STB_FOUND FALSE)
else ()
    # Look for stb_image.h in the specified STB_ROOT directory
    find_path(STB_INCLUDE_DIR
        NAMES stb_image.h
        HINTS ${STB_ROOT}
        NO_DEFAULT_PATH
    )

    # Check find status
    if(STB_INCLUDE_DIR)
        set(STB_FOUND TRUE)
        set(STB_INCLUDE_DIRS ${STB_INCLUDE_DIR})
    else()
        set(STB_FOUND FALSE)
    endif()

    # Hide intermediate variables
    mark_as_advanced(STB_INCLUDE_DIR)
endif ()

# Check find result
if (STB_FOUND)
    # Add library
    message(STATUS "Found STB library")
    # Add library
    add_library(STB INTERFACE IMPORTED)
    # Add alias to it
    add_library(STB::STB ALIAS STB)
    # Setup header files
    set_target_properties(STB PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
        "${STB_INCLUDE_DIRS}"
    )
else ()
    # If it is required, show infomations.
    if (STB_FIND_REQUIRED)
        message(FATAL_ERROR "Fail to find STB library.")
    endif ()
endif ()
