cmake_minimum_required(VERSION 3.10)
project(iir_queue_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/sqlite-autoconf-3500400
)

file(GLOB SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/libs/sqlite-autoconf-3500400/sqlite3.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} TgBot ssl crypto pthread curl sqlite3)

set_source_files_properties(${CMAKE_SOURCE_DIR}/libs/sqlite-autoconf-3500400/sqlite3.c PROPERTIES LANGUAGE C)