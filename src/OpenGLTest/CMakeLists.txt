
project(OpenGLTest)

link_libraries(OpenGL
               ${CMAKE_DL_LIBS})

add_executable(${PROJECT_NAME}
               "src/Main.cpp")

if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif()

