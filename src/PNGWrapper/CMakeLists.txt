project(PNGWrapper VERSION 1.0.0 DESCRIPTION "C++ wrapper for libpng")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include_directories(${PROJECT_NAME} ${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})

add_library (${PROJECT_NAME} 
             "src/PNGWrapper.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} png_static fmt)

