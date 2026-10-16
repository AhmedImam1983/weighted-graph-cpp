cmake_minimum_required(VERSION 3.20)
project(WeightedGraphAlgorithms VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(graph INTERFACE)
target_include_directories(graph INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(graph INTERFACE /W4 /permissive-)
else()
    target_compile_options(graph INTERFACE -Wall -Wextra -Wpedantic)
endif()

add_executable(graph_demo src/main.cpp)
target_link_libraries(graph_demo PRIVATE graph)

include(CTest)
if(BUILD_TESTING)
    add_executable(graph_tests tests/test_graph.cpp)
    target_link_libraries(graph_tests PRIVATE graph)
    add_test(NAME graph_tests COMMAND graph_tests)
endif()
