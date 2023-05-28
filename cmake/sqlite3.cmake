#
# includes the sqlite library as 'SQLite3'
# should only be used through the wrapper project 'SQLiteDB'
#

FetchContent_Declare(
  sqlite

  #GIT_REPOSITORY https://github.com/sqlite/sqlite.git
  #GIT_TAG        master
  
  GIT_REPOSITORY https://github.com/algoritnl/sqlite-amalgamation-cmake-buildsystem.git
  GIT_TAG        version-3.42.0
# GIT_TAG        main

  SOURCE_DIR sqlite-src
  BINARY_DIR sqlite-build

  PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3/CMakeLists.txt <SOURCE_DIR>/CMakeLists.txt

  UPDATE_DISCONNECTED ON
)

MESSAGE(STATUS "
*************************************
*** 
***  sqlite3
*** 
*************************************")

#include(DebugVariables)

set(ZLIB_LIBRARY zlibstatic)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})

SET(BUILD_TESTING OFF)
SET(BUILD_SQLITE_EXE OFF)

FetchContent_MakeAvailable(sqlite)

SET(BUILD_TESTING ON)
