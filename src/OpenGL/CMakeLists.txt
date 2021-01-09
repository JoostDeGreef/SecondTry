
project(OpenGL VERSION 1.0.0 DESCRIPTION "OpenGL functions")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

link_libraries(Core
               fmt
               glad
               glfw)

add_library (${PROJECT_NAME} # SHARED 
             "src/OpenGL.cpp"
             "src/Window.cpp")

# directories where to search for header files
target_include_directories(${PROJECT_NAME}
    PUBLIC include
	PRIVATE include/internal)

