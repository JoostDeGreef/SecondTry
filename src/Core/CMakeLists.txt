project(Core VERSION 1.0.0 DESCRIPTION "Core functions")

add_library (${PROJECT_NAME}
             "src/Core.cpp"
             "src/Log.cpp")

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt)
