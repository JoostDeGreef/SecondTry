#
# BZIP2
#

#include(DebugVariables)

FetchContent_Declare(
  bzip2
 
  GIT_REPOSITORY https://github.com/libarchive/bzip2.git
  GIT_TAG        master

  SOURCE_DIR bzip2-build
  BINARY_DIR bzip2-build
)

set(ENABLE_STATIC_LIB ON)

FetchContent_MakeAvailable(bzip2)



