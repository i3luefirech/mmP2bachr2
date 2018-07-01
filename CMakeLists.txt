cmake_minimum_required(VERSION 3.10)
project(mmP2bachr2)

set(CMAKE_CXX_STANDARD 11)
IF(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF()

add_executable(mmP2bachr2 main.cpp realmouse.cpp realmouse.h)

IF(UNIX)
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(mmP2bachr2 ${X11_LIBRARIES})
ENDIF()