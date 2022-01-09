#
# includes the sqlite library as 'SQLite3'
# should only be used through the wrapper project 'SQLiteDB'
#

FetchContent_Declare(
  sqlite

  #GIT_REPOSITORY https://github.com/sqlite/sqlite.git
  #GIT_TAG        master
  
  GIT_REPOSITORY https://github.com/algoritnl/sqlite-amalgamation.git
  GIT_TAG        main
)

MESSAGE(STATUS "
*************************************
*** 
***  sqlite3
*** 
*************************************")

#include(DebugVariables)

SET(BUILD_TESTING OFF)

FetchContent_MakeAvailable(sqlite)

SET(BUILD_TESTING ON)
