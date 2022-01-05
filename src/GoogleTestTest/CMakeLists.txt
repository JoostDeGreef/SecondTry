
project(GoogleTestTest)

add_executable(${PROJECT_NAME}
               "src/OutputTest.cpp")

target_link_libraries(
        ${PROJECT_NAME} 
        GoogleTest)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME})
