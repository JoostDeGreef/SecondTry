# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.18)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget SQLite3)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target SQLite3
add_library(SQLite3 STATIC IMPORTED)

set_target_properties(SQLite3 PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "\$<\$<BOOL:OFF>:SQLITE_OMIT_DEPRECATED>;\$<\$<BOOL:OFF>:SQLITE_OMIT_LOAD_EXTENSION>"
  INTERFACE_INCLUDE_DIRECTORIES "C:/Src/SecondTry/out/build/x64-Debug/_deps/sqlite-src/source"
  INTERFACE_LINK_LIBRARIES "\$<\$<NOT:\$<BOOL:OFF>>:Threads::Threads>;\$<LINK_ONLY:\$<\$<BOOL:OFF>:Threads::Threads>>;\$<\$<NOT:\$<BOOL:OFF>>:>;\$<LINK_ONLY:\$<\$<BOOL:OFF>:>>"
)

# Import target "SQLite3" for configuration "Debug"
set_property(TARGET SQLite3 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SQLite3 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "C:/Src/SecondTry/out/build/x64-Debug/lib/sqlite3-dbg.lib"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
