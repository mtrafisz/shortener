cmake_minimum_required (VERSION 3.5)
project (shortener C)

set(CMAKE_C_STANDARD 99)

execute_process(
    COMMAND git submodule update --init --recursive --depth 1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(third-party/json-c)
add_subdirectory(third-party/comet)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

file (GLOB LOCAL_SOURCES "src/*.c")

add_executable(${PROJECT_NAME} ${LOCAL_SOURCES})
set (LOCAL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories (${PROJECT_NAME} PUBLIC
    ${LOCAL_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/json-c
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/comet/include
)
target_link_libraries( ${PROJECT_NAME}
    json-c
    comet
    ${SQLITE3_LIBRARIES}
)

file (GLOB EXAMPLES "tests/*.c")
foreach (EXAMPLE ${EXAMPLES})
    get_filename_component (EXAMPLE_NAME ${EXAMPLE} NAME_WE)
    add_executable (${EXAMPLE_NAME} ${EXAMPLE})
    target_link_libraries (${EXAMPLE_NAME} ${PROJECT_NAME})
endforeach ()
