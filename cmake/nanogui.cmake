#
# nanoGUI
#

#include(DebugVariables)

FetchContent_Declare(
  nanogui
 
  GIT_REPOSITORY https://github.com/mitsuba-renderer/nanogui
  GIT_TAG        v0.2.0
# GIT_TAG        master

  SOURCE_DIR nanogui-src
  BINARY_DIR nanogui-build

  UPDATE_DISCONNECTED ON
)

MESSAGE(STATUS "
*************************************
*** 
***  nanogui
*** 
*************************************")

#include(DebugVariables)

#set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install anything" FORCE)
SET(NANOGUI_BUILD_GLFW OFF)
SET(NANOGUI_INSTALL OFF)

FetchContent_MakeAvailable(nanogui)

target_include_directories(nanogui PUBLIC ${glfw_SOURCE_DIR}/include)
target_link_libraries(nanogui PUBLIC glfw)

#SET(BUILD_TESTING ON)



