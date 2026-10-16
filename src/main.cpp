#include "graph/graph.hpp"
#include "graph/monotonic_stack.hpp"

#include <iostream>
#include <vector>

int main() {
    const std::vector<GraphEdge<int>> edges{
        {0, 3, 20},
        {0, 1, 4},
        {1, 3, 5},
        {0, 2, 2},
        {2, 3, 10}
    };

    const Graph<int> graph(6, edges);

    std::cout << "Shortest cost 0 -> 3: " << graph.dijkstra(0, 3) << '\n';
    std::cout << "Shortest cost 0 -> 2: " << graph.dijkstra(0, 2) << '\n';
    std::cout << "Shortest cost 0 -> 5: " << graph.dijkstra(0, 5) << '\n';

    const auto path = graph.getPath(0, 3);
    std::cout << "Fewest-edge path 0 -> 3: ";
    for (std::size_t i = 0; i < path.size(); ++i) {
        if (i != 0) std::cout << " -> ";
        std::cout << path[i];
    }
    std::cout << '\n';

    const auto nextGreater = nextGreaterElements({2, 1, 3, 4, 2});
    std::cout << "Next greater values: ";
    for (int value : nextGreater) std::cout << value << ' ';
    std::cout << '\n';
}
