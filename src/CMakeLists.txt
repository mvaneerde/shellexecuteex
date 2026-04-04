cmake_minimum_required(VERSION 3.21)

if(DEFINED PROJECT_VERSION)
    set(VERSION_STR ${PROJECT_VERSION})
else()
    set(VERSION_STR "0.0.0")
endif()

message(STATUS "Building version: ${VERSION_STR}")

project(ShellExecuteEx VERSION ${VERSION_STR} LANGUAGES CXX)

add_executable(${PROJECT_NAME} shellexecuteex.cpp prefs.cpp)
if (MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()
