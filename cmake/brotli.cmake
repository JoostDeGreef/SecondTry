#
# BROTLI
#

FetchContent_Declare(
  brotli
 
  GIT_REPOSITORY https://github.com/google/brotli.git
  GIT_TAG        v1.0.9
# GIT_TAG        master

  SOURCE_DIR brotli-src
  BINARY_DIR brotli-build

  UPDATE_DISCONNECTED ON
)

MESSAGE(STATUS "
*************************************
*** 
***  brotli 
*** 
*************************************")

#include(DebugVariables)

SET(BROTLI_BUNDLED_MODE ON)
SET(BROTLI_DISABLE_TESTS ON)

FetchContent_MakeAvailable(brotli)



