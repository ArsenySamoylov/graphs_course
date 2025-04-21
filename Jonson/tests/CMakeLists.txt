cmake_minimum_required(VERSION 3.20)
enable_testing()

set(TEST_EXE jonson-unit-tests.out)

set(TESTS_SRC
            dijkstra-unit-tests.cpp
            bellman-ford-unit-tests.cpp
            jonson-unit-tests.cpp
            )

include_directories(../src)
add_executable(${TEST_EXE} ${TESTS_SRC})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
target_link_libraries(${TEST_EXE} GTest::gtest_main)
gtest_discover_tests(${TEST_EXE})