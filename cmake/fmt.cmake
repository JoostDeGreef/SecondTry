#
# FMT
#

FetchContent_Declare(
  fmt

  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        9.1.0
# GIT_TAG        master

  SOURCE_DIR fmt-src
  BINARY_DIR fmt-build

  UPDATE_DISCONNECTED ON
)

MESSAGE(STATUS "
*************************************
*** 
***  fmt 
*** 
*************************************")

#include(DebugVariables)

FetchContent_MakeAvailable(fmt)


