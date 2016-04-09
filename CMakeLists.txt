cmake_minimum_required(VERSION 3.5)
include(CheckFunctionExists)
include(CheckSymbolExists)

check_function_exists(mmap HAVE_MMAP)
if(HAVE_MMAP)
    check_symbol_exists(MAP_ANONYMOUS sys/mman.h HAVE_MAP_ANONYMOUS)
    if(NOT HAVE_MAP_ANONYMOUS)
        check_symbol_exists(MAP_ANON sys/mman.h HAVE_MAP_ANON)
    endif()
    check_function_exists(mremap HAVE_MREMAP)
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Werror -std=c99")

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_library(allocation src/allocation.c)

add_library(coroutine src/coroutine.c)
target_link_libraries(coroutine LINK_PUBLIC allocation)

add_library(default_allocator src/default_allocator.c)
set(ALLOCATORS default_allocator)

add_library(heap src/heap.c)
target_link_libraries(heap LINK_PUBLIC allocation)

if(HAVE_MMAP)
    add_library(mmap_allocator src/mmap_allocator.c)
    set(ALLOCATORS ${ALLOCATORS} mmap_allocator)
endif()

add_executable(test-allocation test/allocation.c)
target_link_libraries(test-allocation LINK_PUBLIC allocation ${ALLOCATORS})
add_executable(test-coroutine test/coroutine.c)
target_link_libraries(test-coroutine LINK_PUBLIC coroutine ${ALLOCATORS})
add_executable(test-heap test/heap.c)
target_link_libraries(test-heap LINK_PUBLIC heap ${ALLOCATORS})
