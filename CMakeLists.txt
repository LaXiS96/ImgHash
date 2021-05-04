cmake_minimum_required(VERSION 3.16)
project(ImgHash)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

set(PHASH_DYNAMIC OFF CACHE BOOL "Build pHash dynamic library")
set(PHASH_STATIC ON CACHE BOOL "Build pHash dynamic library")
add_subdirectory(lib/pHash)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(include)

add_executable(ImgHash src/main.c)

target_link_libraries(ImgHash
                      PUBLIC pHash
                      PUBLIC Threads::Threads)

target_include_directories(ImgHash
                           PUBLIC "${pHash_SOURCE_DIR}/bindings/c")
