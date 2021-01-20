#
# freetype
#

FetchContent_Declare(
  freetype
 
  GIT_REPOSITORY https://github.com/freetype/freetype2.git
  GIT_TAG        master
)

set(ZLIB_LIBRARY zlibstatic)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})

set(PNG_LIBRARY png_static)
set(PNG_PNG_INCLUDE_DIR ${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})

#BZIP2_LIBRARIES
#BZIP2_INCLUDE_DIR

set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install anything" FORCE)

FetchContent_MakeAvailable(freetype)

