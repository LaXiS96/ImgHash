cmake_minimum_required(VERSION 3.16)
project(ImgHash)

# set(CMAKE_C_STANDARD 99)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(PHASH_DYNAMIC OFF CACHE BOOL "Build pHash dynamic library")
set(PHASH_STATIC ON CACHE BOOL "Build pHash dynamic library")
set(PHASH_C_BINDINGS ON CACHE BOOL "Include C bindings")
add_subdirectory(lib/pHash)

add_executable(ImgHash src/main.c)
target_include_directories(ImgHash
    PRIVATE include
)
target_link_libraries(ImgHash
    PRIVATE Threads::Threads
    PRIVATE pHash
)
