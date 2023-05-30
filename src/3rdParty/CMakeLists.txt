# 3rd party libraries    
include(FetchContent)

# Remote code

include(googletest)
include(fmt)
include(glfw)
include(zlib)
include(sqlite3)
include(bzip2)
include(brotli)
include(png)
include(freetype2)

# NanoGUI is not usefull
#include(nanogui)

# local code

add_subdirectory ("glad")

