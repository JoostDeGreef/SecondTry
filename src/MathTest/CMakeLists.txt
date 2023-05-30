
project(MathTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

add_executable(${PROJECT_NAME}
               "src/Matrix.cpp"
               "src/MatrixTest.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE GoogleTest)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME})
