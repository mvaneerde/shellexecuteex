cmake_minimum_required(VERSION 3.21)
project(ShellExecuteEx LANGUAGES CXX)

# ANSI is dead
add_compile_definitions(UNICODE _UNICODE)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

# for the ShellExecutExW API implementation
link_libraries(Shell32)

# pass in version number
if(DEFINED PROJECT_VERSION)
    set(VERSION_STR ${PROJECT_VERSION})
elseif(DEFINED ENV{PROJECT_VERSION})
    set(VERSION_STR $ENV{PROJECT_VERSION})
else()
    set(VERSION_STR "0.0.0")
endif()
message(STATUS "Building version: ${VERSION_STR}")

# embed the version in resources.rc
string(REPLACE "." ";" VERSION_LIST ${VERSION_STR})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)
configure_file(
    ${CMAKE_SOURCE_DIR}/resources.rc.in
    ${CMAKE_BINARY_DIR}/resources.rc
    @ONLY
)

# build code into a library that can be consumed by the product and tests
add_library(shellexecuteex_lib prefs.cpp)

add_executable(${PROJECT_NAME} main.cpp ${CMAKE_BINARY_DIR}/resources.rc)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION_STR})

# for pulling in resources.h from the generated resources.rc
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE shellexecuteex_lib)

# build tests
enable_testing()

include(FetchContent)

# Force GTest to use the Dynamic CRT to match the project settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "tests/*.cpp")

add_executable(tests ${TEST_SOURCES})
if (MSVC)
    target_include_directories(tests
        PRIVATE ${CMAKE_SOURCE_DIR}
    )
endif()
target_link_libraries(tests PRIVATE shellexecuteex_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
