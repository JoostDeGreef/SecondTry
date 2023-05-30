project(GoogleTest VERSION 1.0.0 DESCRIPTION "C++ wrapper for gtest")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library (${PROJECT_NAME}
             "src/ConsoleOutput.cpp"
             "src/VisualStudioOutput.cpp"
             "src/GoogleTestMain.cpp")


#include(DebugVariables)

target_include_directories(${PROJECT_NAME} PUBLIC ${gtest_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC gtest)

