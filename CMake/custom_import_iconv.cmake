if (WIN32)
    # In Windows, we should not import Iconv.
    # Send a notice to programmer.
    message("Windows environment detected, skip finding Iconv!")
else ()
    # In non-Windows, we simply import Iconv from CMake preset.
    # It will produce Iconv::Iconv target for including and linking.
    find_package(Iconv REQUIRED)
endif ()
