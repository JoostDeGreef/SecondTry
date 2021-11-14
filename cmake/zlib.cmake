#
# ZLIB
#

#include(DebugVariables)

FetchContent_Declare(
  zlib
 
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        master

  SOURCE_DIR zlib-build
  BINARY_DIR zlib-build
)

set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install anything" FORCE)

FetchContent_MakeAvailable(zlib)

#include_directories(${zlib_SOURCE_DIR})
#include_directories(${zlib_BINARY_DIR})
#if(NOT zlib_POPULATED)
#	FetchContent_Populate(zlib)
#	add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
#endif()


