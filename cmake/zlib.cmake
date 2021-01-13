#
# ZLIB
#

FetchContent_Declare(
  zlib
 
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        master
)

set(SKIP_INSTALL_ALL ON CACHE BOOL "Don't install anything" FORCE)

FetchContent_MakeAvailable(zlib)

