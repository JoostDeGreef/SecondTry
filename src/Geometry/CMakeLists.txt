project(Geometry VERSION 1.0.0 DESCRIPTION "Geometry objects and functions")

add_library (${PROJECT_NAME}
             "src/Geometry.cpp"
             "src/Shape/Shape.cpp"
             "src/Shape/Construct.cpp"
             "src/Face.cpp"
             "src/Edge.cpp"
             "src/Polygon.cpp"
             "src/TriangulatedPolygon2D.cpp"
             "src/Utilities.cpp"
             "src/Constants.cpp"
             "src/Operations/Operations2D.cpp"
             "src/Operations/Operations3D.cpp"
             "src/Operations/Sweeper.cpp"
             "src/Operations/SweepLine.cpp")

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(${PROJECT_NAME} PUBLIC Core)

