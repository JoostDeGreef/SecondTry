
project(PNGWrapperTest)

add_executable (${PROJECT_NAME}
                "src/SmokeTest.cpp")

target_link_libraries(
        ${PROJECT_NAME} 
        GoogleTest
        PNGWrapper)
