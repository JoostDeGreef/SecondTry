
project(3rdPartyTest)

add_executable (${PROJECT_NAME}
                "GoogleTest/OutputTest.cpp")

target_link_libraries(${PROJECT_NAME} GoogleTest)
