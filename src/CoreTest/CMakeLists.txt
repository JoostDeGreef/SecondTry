project(CoreTest)

add_executable (${PROJECT_NAME}
                "VectorTest.cpp"
                "QuaternionTest.cpp"
                "CoreTest.cpp"
                "SmartPtrStoreTest.cpp"
                "LogTest.cpp")

target_link_libraries(${PROJECT_NAME} GoogleTest Core)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME})

