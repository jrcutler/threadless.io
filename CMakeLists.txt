cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Werror -std=c99")

add_library (coroutine src/coroutine.c)
target_include_directories (coroutine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (test-coroutine test/coroutine.c)
target_include_directories (coroutine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (test-coroutine LINK_PUBLIC coroutine)
