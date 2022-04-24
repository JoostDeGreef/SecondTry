project(GeometryTest)

add_executable (${PROJECT_NAME}
                "GeometryTest.cpp"
                "OperationsTest.cpp"
                "PolygonTest.cpp"
                "ShapeTest.cpp"
                "SweepLineTest.cpp"
                "SweeperTest.cpp"
                "Vector2dCompareTest")

target_link_libraries(${PROJECT_NAME} GoogleTest Geometry)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME})

