#
# GLFW3
#

FetchContent_Declare(
  glfw
 
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)

FetchContent_MakeAvailable(glfw)
