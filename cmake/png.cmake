#
# PNG
#

FetchContent_Declare(
  libpng
 
  GIT_REPOSITORY https://github.com/glennrp/libpng.git
  GIT_TAG        master
)

#include(DebugVariables)

set(ZLIB_LIBRARIES zlib)
set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(PNG_BUILD_ZLIB ON CACHE BOOL "Custom zlib Location")
set(PNG_EXECUTABLES OFF CACHE BOOL "Build libpng executables")
set(PNG_TESTS OFF CACHE BOOL "Build libpng tests")
set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install anything")

FetchContent_MakeAvailable(libpng)


#add_library(ZLIB INTERFACE)
#target_include_directories(ZLIB INTERFACE ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
#target_link_libraries(ZLIB INTERFACE zlib)

