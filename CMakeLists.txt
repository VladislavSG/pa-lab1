cmake_minimum_required(VERSION 3.16)
project(lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(lab1 main.cpp quicksort.h seq_quicksort.h par_quicksort.h)

find_package(TBB)
target_link_libraries(lab1 TBB::tbb)
