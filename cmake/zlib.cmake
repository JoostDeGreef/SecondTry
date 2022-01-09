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

  PATCH_COMMAND  git restore . && git apply --ignore-space-change --ignore-whitespace ${CMAKE_CURRENT_SOURCE_DIR}/zlib.patch
)

MESSAGE(STATUS "
*************************************
*** 
***  zlib
*** 
*************************************")

#include(DebugVariables)

set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install anything" FORCE)
SET(BUILD_TESTING OFF)

FetchContent_MakeAvailable(zlib)

SET(BUILD_TESTING ON)



