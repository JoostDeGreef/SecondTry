
project(PlanetBuilders VERSION 0.0.1 DESCRIPTION "Planet Builders")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(lib${PROJECT_NAME}
               "src/UI.cpp"
               "src/UICallbacks.cpp")
  
target_include_directories(lib${PROJECT_NAME}
               PUBLIC src)

target_link_libraries(lib${PROJECT_NAME}
               OpenGL
               Core
               ${CMAKE_DL_LIBS})

add_executable(${PROJECT_NAME}
               "src/Main.cpp")
 
target_link_libraries(${PROJECT_NAME} 
               lib${PROJECT_NAME})

if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif()

