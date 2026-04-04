cmake_minimum_required(VERSION 3.21)

# grab the latest version from the tag or default to 0.0.0
execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE LATEST_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(NOT LATEST_TAG)
    set(LATEST_TAG "v0.0.0")
endif()

# Strip leading 'v'
string(REGEX REPLACE "^v" "" VERSION_STR "${LATEST_TAG}")

message(STATUS "Building version: ${VERSION_STR}")

project(ShellExecuteEx VERSION ${VERSION_STR} LANGUAGES CXX)

add_executable(${PROJECT_NAME} shellexecuteex.cpp prefs.cpp)
if (MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()
