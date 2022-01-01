project(CoreTest)

add_executable (${PROJECT_NAME}
                "VectorTest.cpp"
                "QuaternionTest.cpp"
                "CoreTest.cpp"
                "LogTest.cpp")

target_link_libraries(${PROJECT_NAME} GoogleTest Core)

