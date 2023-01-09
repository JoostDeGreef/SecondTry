
project(OpenGLTest)

add_executable (${PROJECT_NAME}
                "src/Menu.cpp")

target_link_libraries(
        ${PROJECT_NAME} 
        GoogleTest
        OpenGL)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME})
