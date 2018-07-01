cmake_minimum_required(VERSION 3.10)
project(mmP2bachr2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(mmP2bachr2 main.cpp realmouse.cpp realmouse.h)