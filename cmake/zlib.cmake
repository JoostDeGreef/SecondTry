#
# ZLIB
#

FetchContent_Declare(
  zlib
 
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(zlib)

#add_library(ZLIB INTERFACE)
#target_include_directories(ZLIB INTERFACE ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
#target_link_libraries(ZLIB INTERFACE zlib)

