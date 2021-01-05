include(Common)

project(SQLiteTest)

link_libraries(GoogleTest
               SQLiteDB
               ${CMAKE_DL_LIBS})

add_executable (${PROJECT_NAME}
                "SQLiteTest.cpp")

