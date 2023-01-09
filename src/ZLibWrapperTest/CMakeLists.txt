
project(ZLibWrapperTest)

add_executable (${PROJECT_NAME}
                "src/SmokeTest.cpp")

target_link_libraries(
        ${PROJECT_NAME} 
        GoogleTest
        ZLibWrapper)

add_test(NAME ${PROJECT_NAME}
        COMMAND ${PROJECT_NAME})
