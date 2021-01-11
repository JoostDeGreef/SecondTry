
project(GoogleTestTest)

add_executable (${PROJECT_NAME}
                "src/OutputTest.cpp")

target_link_libraries(
        ${PROJECT_NAME} 
        GoogleTest)
