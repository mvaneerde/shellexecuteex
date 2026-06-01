cmake_minimum_required(VERSION 3.21)

# pull version number from the command line or environment
if(DEFINED PROJECT_VERSION)
    set(VERSION_STR ${PROJECT_VERSION})
elseif(DEFINED ENV{PROJECT_VERSION})
    set(VERSION_STR $ENV{PROJECT_VERSION})
else()
    set(VERSION_STR "0.0.0")
endif()
message(STATUS "Building version: ${VERSION_STR}")

project(ShellExecuteEx VERSION ${VERSION_STR} LANGUAGES CXX)

# ANSI is dead
add_compile_definitions(UNICODE _UNICODE)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

# for the ShellExecuteExW API implementation
link_libraries(Shell32)

# embed the version in resources.rc
set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_PATCH ${PROJECT_VERSION_PATCH})
configure_file(
    ${CMAKE_SOURCE_DIR}/resources.rc.in
    ${CMAKE_BINARY_DIR}/resources.rc
    @ONLY
)

# build code into a library that can be consumed by the product and tests
add_library(
    shellexecuteex_lib
    cleanup.cpp
    known-folders.cpp
    main-internal.cpp
    prefs.cpp
    usage.cpp
    show-window-command.cpp
    windows-api.cpp
)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    ${CMAKE_BINARY_DIR}/resources.rc
)
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
include(GoogleTest)

add_executable(
    unit_tests
    tests/unit/known-folders.cpp
    tests/unit/main.cpp
    tests/unit/show-window-command.cpp
    tests/unit/usage.cpp
    tests/test-helpers.cpp
    tests/test-main.cpp)
if (MSVC)
    target_include_directories(unit_tests
        PRIVATE ${CMAKE_SOURCE_DIR}
        PRIVATE ${CMAKE_SOURCE_DIR}/tests
    )
endif()
target_link_libraries(unit_tests PRIVATE shellexecuteex_lib GTest::gtest_main GTest::gmock_main)

gtest_discover_tests(unit_tests PROPERTIES LABELS "unit")

add_executable(
    functional_tests
    tests/functional/prefs-flags.cpp
    tests/functional/prefs-item-id-list-from-display-name.cpp
    tests/functional/prefs-item-id-list-from-known-folder-id.cpp
    tests/functional/prefs-relay-exit-code.cpp
    tests/functional/prefs-scenarios.cpp
    tests/functional/prefs-show.cpp
    tests/functional/prefs-string-arguments.cpp
    tests/functional/run-scenarios.cpp
    tests/functional/usage.cpp
    tests/test-helpers.cpp
    tests/test-main.cpp)
if (MSVC)
    target_include_directories(functional_tests
        PRIVATE ${CMAKE_SOURCE_DIR}
        PRIVATE ${CMAKE_SOURCE_DIR}/tests
    )
endif()
target_link_libraries(functional_tests PRIVATE shellexecuteex_lib GTest::gtest_main GTest::gmock_main)

gtest_discover_tests(functional_tests PROPERTIES LABELS "functional")
