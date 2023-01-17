#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SQLite::SQLite3" for configuration "Debug"
set_property(TARGET SQLite::SQLite3 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SQLite::SQLite3 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/sqlite3-dbg.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SQLite::SQLite3 )
list(APPEND _IMPORT_CHECK_FILES_FOR_SQLite::SQLite3 "${_IMPORT_PREFIX}/lib/sqlite3-dbg.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
