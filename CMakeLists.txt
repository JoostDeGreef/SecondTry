cmake_minimum_required (VERSION 3.14)

# These are just toy projects, make that clear
project ("SecondTry" VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are a bad idea and not allowed.")
endif ()

# for visual studio (FMT needs this)
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

# IF(CMAKE_COMPILER_IS_GNUCXX)
#   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info-vec-all -O3")
# ENDIF(CMAKE_COMPILER_IS_GNUCXX)
# IF(CMAKE_COMPILER_IS_GNUCXX)
#   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
# ENDIF(CMAKE_COMPILER_IS_GNUCXX)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Output will land here
SET(GLOBAL_PREFIX
    ${PROJECT_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE} CACHE
    PATH "Directory where all the output files go." FORCE)
SET(GLOBAL_BUILD_DIRECTORY
    ${GLOBAL_PREFIX}/build CACHE
    PATH "Directory where all the builds go." FORCE)
SET(CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake CACHE
    PATH "Directory where .cmake files are stored." FORCE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# all non-interactive unit tests will be run
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure" "-V")
enable_testing()

# Include sub-projects.
add_subdirectory ("src")

