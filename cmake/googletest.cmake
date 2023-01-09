#
# includes Google test framework as 'gtest'
# should only be used through the wrapper project 'GoogleTest'
#

FetchContent_Declare(
  googletest

  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
# GIT_TAG        main
)

MESSAGE(STATUS "
*************************************
*** 
***  googletest
*** 
*************************************")

#include(DebugVariables)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "Disable BUILD_GMOCK" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Disable INSTALL_GTEST" FORCE)

FetchContent_MakeAvailable(googletest)
