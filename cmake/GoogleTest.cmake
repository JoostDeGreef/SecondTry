#
# googletest
#

FetchContent_Declare(
  GoogleTest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        master
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "Disable BUILD_GMOCK" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Disable INSTALL_GTEST" FORCE)
FetchContent_MakeAvailable(GoogleTest)
add_library(GoogleTest INTERFACE)
target_link_libraries(GoogleTest INTERFACE gtest gtest_main)
