cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Werror -std=c99")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_library (allocation src/allocation.c)

add_library (coroutine src/coroutine.c)
target_link_libraries (coroutine LINK_PUBLIC allocation)

add_library (default_allocator src/default_allocator.c)

add_executable (test-coroutine test/coroutine.c)
target_link_libraries (test-coroutine LINK_PUBLIC coroutine default_allocator)
