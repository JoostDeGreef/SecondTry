
project(OpenGL VERSION 1.0.0 DESCRIPTION "OpenGL functions")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

link_libraries(Core
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
endif()
if (WIN32)
    message(STATUS "${Green} # - This Is a Windows Machine - # ${ColourReset}")
    set(OSSPECIFIC "src/OSSpecific-win32.cpp")
endif()

add_library (${PROJECT_NAME} # SHARED 
             "src/OpenGL.cpp"
             "src/Window.cpp"
             ${OSSPECIFIC}
             "src/Font.cpp" 
             "src/Shader.cpp")


# directories where to search for header files
target_include_directories(${PROJECT_NAME}
    PUBLIC include
	PRIVATE include/internal)

