cmake_minimum_required(VERSION 3.20)

set(PROJECT Jonson)
project(${PROJECT})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_FILES src/main.cpp)

if ( UNIX )
set(CMAKE_CXX_FLAGS_DEBUG "-g -Werror -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls")
endif()

add_executable(${PROJECT} ${SRC_FILES})

# tests #
add_subdirectory(tests)

