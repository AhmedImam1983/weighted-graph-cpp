# Weighted Graph Algorithms (C++17)

A small C++17 project demonstrating graph representation, traversal, shortest paths, connected components, and monotonic-stack algorithms. It is organized as a reusable header-only graph component, a demo executable, and a separate regression-test executable.

## Features

- Undirected weighted graph using an adjacency list
- Vertex validation when adding edges
- BFS reachability, traversal, and fewest-edge path reconstruction
- Dijkstra shortest-path cost with a min-priority queue
- Connected-component discovery
- Bitwise-AND cost across a connected component
- Monotonic-stack examples: distance to next greater element and next greater value
- CMake build and CTest regression tests
- GitHub Actions workflow for Linux CI

## Requirements

- C++17 compiler (GCC 9+, Clang 10+, or a recent MSVC)
- CMake 3.20+

## Build and run (Linux / WSL)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/graph_demo
```

## Build and run (Visual Studio on Windows)

Open the folder containing `CMakeLists.txt` in Visual Studio 2022, select a CMake configuration, build `graph_demo` and `graph_tests`, then run the tests through Test Explorer or CTest.

Alternatively, from a Developer PowerShell:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\graph_demo.exe
```

The exact executable path can vary by generator.

## Example output

```text
Shortest cost 0 -> 3: 9
Shortest cost 0 -> 2: 2
Shortest cost 0 -> 5: -1
Fewest-edge path 0 -> 3: 0 -> 3
Next greater values: 3 3 4 -1 -1
```

## Design notes

- The graph is **undirected**: every edge is stored in both directions.
- `getPath()` uses BFS, so it minimizes the number of edges, not the sum of edge weights.
- `dijkstra()` requires all edge weights to be non-negative. It throws `std::domain_error` if a negative edge exists and returns `-1` for invalid endpoints or an unreachable destination.
- `Weight` is a signed integral type, because `-1` is used as a sentinel.
- `findMinimumBitwiseAndCost()` returns the bitwise AND of all edge weights in the shared connected component. It returns `-1` for invalid endpoints or vertices in different components; an isolated vertex has component cost `0`.
- `distanceToNextGreater()` returns the index distance to the next strictly greater value, or `0` when none exists.
- `nextGreaterElements()` returns the next strictly greater value, or `-1` when none exists.

## Project layout

```text
.
├── .github/workflows/ci.yml
├── include/graph/graph.hpp
├── include/graph/monotonic_stack.hpp
├── src/main.cpp
├── tests/test_graph.cpp
├── CMakeLists.txt
└── README.md
```

## Scope

This is a focused learning and demonstration project, not a production graph library. The tests cover representative normal and error cases, but they are not exhaustive. Future improvements could include path reconstruction for Dijkstra, property-based tests, and a more expressive result type than the `-1` sentinel.
