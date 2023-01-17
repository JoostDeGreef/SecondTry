#
# FMT
#

FetchContent_Declare(
  fmt

  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        9.1.0
# GIT_TAG        master
)

MESSAGE(STATUS "
*************************************
*** 
***  fmt 
*** 
*************************************")

#include(DebugVariables)

FetchContent_MakeAvailable(fmt)


