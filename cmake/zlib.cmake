#
# ZLIB
#

#include(DebugVariables)

FetchContent_Declare(
  zlib
 
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        v1.2.13
# GIT_TAG        master

  SOURCE_DIR zlib-src
  BINARY_DIR zlib-build

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/zlib/CMakeLists.txt <SOURCE_DIR>/CMakeLists.txt

  UPDATE_DISCONNECTED ON
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



