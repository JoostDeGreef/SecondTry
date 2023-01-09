
project(OpenGL VERSION 1.0.0 DESCRIPTION "OpenGL functions")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

link_libraries(Core
               Geometry
               fmt
               glad
               glfw
               freetype)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (APPLE)
    message(STATUS "${Green} # - This Is an Apple Machine - # ${ColourReset}")
    set(OSSPECIFIC "src/OSSpecific-apple.cpp")
endif()
if (LINUX)
    message(STATUS "${Green} # - This Is a Linux Machine - # ${ColourReset}")
    set(OSSPECIFIC "src/OSSpecific-X11.cpp")
    link_libraries(X11)
endif()
if (WIN32)
    message(STATUS "${Green} # - This Is a Windows Machine - # ${ColourReset}")
    set(OSSPECIFIC "src/OSSpecific-win32.cpp")
endif()

add_library (${PROJECT_NAME}
             "src/Font.cpp" 
             "src/Mat4.cpp"
             "src/Menu.cpp"             
             "src/OpenGL.cpp"
             "src/Shader.cpp"
             "src/State.cpp"
             "src/GLShape.cpp"
             "src/Window.cpp"
             ${OSSPECIFIC})


# directories where to search for header files
target_include_directories(${PROJECT_NAME}
    PUBLIC include
	PRIVATE include/internal)

