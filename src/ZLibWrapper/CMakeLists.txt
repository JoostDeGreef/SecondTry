project(ZLibWrapper VERSION 1.0.0 DESCRIPTION "C++ wrapper for zlib")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include_directories(${PROJECT_NAME} ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})

add_library (${PROJECT_NAME} 
             "src/ZLibWrapper.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} zlibstatic fmt)

