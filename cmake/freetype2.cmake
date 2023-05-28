#
# freetype
#

FetchContent_Declare(
  freetype
 
  GIT_REPOSITORY https://github.com/freetype/freetype2.git
  GIT_TAG        VER-2-12-1
# GIT_TAG        master

  SOURCE_DIR freetype-src
  BINARY_DIR freetype-build

  UPDATE_DISCONNECTED ON
)

MESSAGE(STATUS "
*************************************
*** 
***  freetype2
*** 
*************************************")

#include(DebugVariables)

set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})

set(BZIP2_INCLUDE_DIR ${bzip2_SOURCE_DIR} ${bzip2_BINARY_DIR})
set(BZIP2_LIBRARIES bz2_static)

set(PNG_LIBRARY png_static)
set(PNG_INCLUDE_DIRS ${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})
set(PNG_PNG_INCLUDE_DIR ${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})

set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "HarfBuzz is not needed" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install anything" FORCE)

FetchContent_MakeAvailable(freetype)

