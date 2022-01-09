#
# PNG
#

FetchContent_Declare(
  png
 
  GIT_REPOSITORY https://github.com/glennrp/libpng.git
  GIT_TAG        master
)

MESSAGE(STATUS "
*************************************
*** 
***  png
*** 
*************************************")

#include(DebugVariables)

set(ZLIB_LIBRARY zlibstatic)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})

set(BZIP2_INCLUDE_DIR ${bzip2_SOURCE_DIR} ${bzip2_BINARY_DIR})
set(BZIP2_LIBRARIES bz2_static)

set(BROTLIDEC_LIBRARIES brotlicommon-static brotlidec-static)
set(BROTLIDEC_INCLUDE_DIRS ${brotli_SOURCE_DIR} ${brotli_BINARY_DIR})

set(PNG_EXECUTABLES OFF CACHE BOOL "Build libpng executables" FORCE)
set(PNG_SHARED OFF CACHE BOOL "Build shared lib" FORCE)
set(PNG_STATIC ON CACHE BOOL "Build static lib" FORCE)
set(PNG_BUILD_ZLIB OFF CACHE BOOL "Custom zlib Location" FORCE)
set(PNG_TESTS OFF CACHE BOOL "Build libpng tests" FORCE)
set(PNG_LIB_NAME "png" CACHE STRING "set the name to something easy to find" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install anything" FORCE)

FetchContent_MakeAvailable(png)


