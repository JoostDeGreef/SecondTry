include(Common)

project(SQLiteDB VERSION 1.0.0 DESCRIPTION "C++ wrapper for SQLite")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

link_libraries(SQLite
               boost)

add_library (${PROJECT_NAME} # SHARED 
             "src/SQLiteDB.cpp"
             "src/SQLiteStatement.cpp"
             "src/SQLiteQuery.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC include)

