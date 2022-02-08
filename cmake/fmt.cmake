#
# FMT
#

FetchContent_Declare(
  fmt

  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        8.1.1
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


