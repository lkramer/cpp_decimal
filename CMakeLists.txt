cmake_minimum_required (VERSION 3.10)
project (interdax_feedhandlers)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS -DKXVER=3)
set(CMAKE_CXX_FLAGS_DEBUG -ggdb)
set(CMAKE_CXX_FLAGS_RELEASE -O3)
set(THREADS_PREFER_PTHREAD_FLAG ON)

include(CTest)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

link_directories(${CMAKE_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

find_package(Threads REQUIRED)

add_compile_options(-Wall -Wpedantic -Wextra)

add_subdirectory(test)

