cmake_minimum_required(VERSION 3.23)
project(convex_hulls)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-O3 -mavx2 -Wl,--no-as-needed -lpthread")

add_executable(convex_hulls main.cpp concurrency/thread_pool/thread_pool.cpp)
