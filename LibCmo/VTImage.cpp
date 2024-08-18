/**
 * \file
 * This file extract all stb-image implementations used by this project.
 * Because std-image is a header-only library.
 * It use macro to control whether extract implementation.
*/

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
