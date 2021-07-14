cmake_minimum_required(VERSION 3.5)

project(salesman LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories ("include")

set(SOURCES main.cpp src/graph.cpp src/vertex.cpp src/hamiltoniancycle.cpp)

find_package (Threads REQUIRED)

add_executable(salesman ${SOURCES})

target_link_libraries (salesman ${CMAKE_THREAD_LIBS_INIT})
