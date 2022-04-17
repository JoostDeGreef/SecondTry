#
# BZIP2
#

#include(DebugVariables)

FetchContent_Declare(
  bzip2
 
  GIT_REPOSITORY https://gitlab.com/bzip2/bzip2.git
  #GIT_TAG        bzip2-1.0.8
  GIT_TAG        master

  SOURCE_DIR bzip2-build
  BINARY_DIR bzip2-build

#  PATCH_COMMAND  git restore . && git apply --ignore-space-change --ignore-whitespace ${CMAKE_CURRENT_SOURCE_DIR}/bzip2.patch
)

MESSAGE(STATUS "
*************************************
*** 
***  bzip2
*** 
*************************************")

#include(DebugVariables)

set(ENABLE_STATIC_LIB ON)
SET(BUILD_TESTING OFF)

FetchContent_MakeAvailable(bzip2)

SET(BUILD_TESTING ON)


