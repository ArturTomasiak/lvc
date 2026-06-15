cmake_minimum_required(VERSION 3.16)

project(lvc LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    app/*.cpp
    app/storage/*.cpp
    external/sqlite/sqlite3.c
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(lvc ${SOURCES})

find_package(ZLIB REQUIRED)
target_link_libraries(lvc PRIVATE ZLIB::ZLIB)

target_include_directories(lvc PRIVATE
    app/
    app/storage
    external/sqlite
)