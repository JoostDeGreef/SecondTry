project(GeometryTest)

add_executable (${PROJECT_NAME}
                "ShapeTest.cpp"
                "PolygonTest.cpp"
                "GeometryTest.cpp"
                "OperationsTest.cpp")

target_link_libraries(${PROJECT_NAME} GoogleTest Geometry)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME})

