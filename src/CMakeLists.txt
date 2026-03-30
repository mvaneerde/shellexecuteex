cmake_minimum_required(VERSION 3.10.0)
project(shellexecuteex VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} shellexecuteex.cpp prefs.cpp)
if (MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()