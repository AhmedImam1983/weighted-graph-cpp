#include "graph/graph.hpp"
#include "graph/monotonic_stack.hpp"

#include <cassert>
#include <stdexcept>
#include <vector>

int main() {
    const std::vector<GraphEdge<int>> edges{
        {0, 3, 20},
        {0, 1, 4},
        {1, 3, 5},
        {0, 2, 2},
        {2, 3, 10},
        {4, 5, 7}
    };
    const Graph<int> graph(7, edges);

    assert(graph.vertexCount() == 7);
    assert(graph.edgeCount() == 6);
    assert(graph.dijkstra(0, 3) == 9);
    assert(graph.dijkstra(0, 2) == 2);
    assert(graph.dijkstra(0, 5) == -1);
    assert(graph.dijkstra(4, 5) == 7);
    assert(graph.dijkstra(2, 2) == 0);

    assert(graph.hasPath(0, 3));
    assert(!graph.hasPath(0, 6));
    assert(!graph.hasPath(-1, 3));
    assert((graph.getPath(0, 3) == std::vector<int>{0, 3}));
    assert((graph.getPath(0, 6).empty()));
    assert((graph.getPath(2, 2) == std::vector<int>{2}));

    const auto traversal = graph.bfsTraverse(0);
    assert(traversal.size() == 4);
    assert(traversal.front() == 0);
    assert(graph.connectedComponents().size() == 3);
    assert(graph.findMinimumBitwiseAndCost(0, 3) >= 0);
    assert(graph.findMinimumBitwiseAndCost(0, 4) == -1);

    assert((distanceToNextGreater({2, 1, 3, 4, 2}) ==
            std::vector<int>{2, 1, 1, 0, 0}));
    assert((nextGreaterElements({2, 1, 3, 4, 2}) ==
            std::vector<int>{3, 3, 4, -1, -1}));
    assert((nextGreaterElements({5, 4, 3}) ==
            std::vector<int>{-1, -1, -1}));
    assert((nextGreaterElements({}).empty()));

    bool rejectedInvalidEdge = false;
    try {
        Graph<int> invalid(3);
        invalid.addEdge({0, 3, 1});
    } catch (const std::out_of_range&) {
        rejectedInvalidEdge = true;
    }
    assert(rejectedInvalidEdge);

    bool rejectedNegativeWeight = false;
    try {
        Graph<int> negative(2, {{0, 1, -4}});
        (void)negative.dijkstra(0, 1);
    } catch (const std::domain_error&) {
        rejectedNegativeWeight = true;
    }
    assert(rejectedNegativeWeight);

    bool rejectedNegativeVertexCount = false;
    try {
        Graph<int> invalid(-1);
    } catch (const std::invalid_argument&) {
        rejectedNegativeVertexCount = true;
    }
    assert(rejectedNegativeVertexCount);
}
