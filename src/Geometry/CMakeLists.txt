project(Geometry VERSION 1.0.0 DESCRIPTION "Geometry objects and functions")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

link_libraries(Core)

add_library (${PROJECT_NAME}
             "src/Geometry.cpp"
             "src/Shape/Shape.cpp"
             "src/Shape/Construct.cpp"
             "src/Face.cpp"
             "src/Edge.cpp"
             "src/Polygon.cpp"
             "src/Utilities.cpp"
             "src/Constants.cpp")

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

