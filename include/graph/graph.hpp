#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

// Undirected weighted graph. Vertex IDs are in [0, vertexCount()).
// Weight is a signed integral type because -1 represents "no path".
template <typename Weight>
struct GraphEdge {
    int u;
    int v;
    Weight weight;
};

template <typename Weight>
struct ConnectedComponent {
    int id{};
    std::vector<int> nodes;
    Weight bitwiseAndCost{};
    bool hasEdges = false;
};

template <typename Weight>
class Graph {
    static_assert(std::is_integral<Weight>::value && std::is_signed<Weight>::value,
                  "Graph Weight must be a signed integral type.");

public:
    explicit Graph(int vertexCount)
        : adjacencyList_(checkedVertexCount(vertexCount)) {}

    Graph(int vertexCount, const std::vector<GraphEdge<Weight>>& edges)
        : Graph(vertexCount) {
        for (const auto& edge : edges) {
            addEdge(edge);
        }
    }

    int vertexCount() const noexcept {
        return static_cast<int>(adjacencyList_.size());
    }

    std::size_t edgeCount() const noexcept {
        std::size_t directedEntries = 0;
        for (const auto& neighbours : adjacencyList_) {
            directedEntries += neighbours.size();
        }
        return directedEntries / 2;
    }

    // Adds an undirected edge. Throws if either endpoint is invalid.
    void addEdge(const GraphEdge<Weight>& edge) {
        validateVertex(edge.u);
        validateVertex(edge.v);
        adjacencyList_[static_cast<std::size_t>(edge.u)].push_back({edge.v, edge.weight});
        adjacencyList_[static_cast<std::size_t>(edge.v)].push_back({edge.u, edge.weight});
    }

    bool hasPath(int start, int end) const {
        if (!isValidVertex(start) || !isValidVertex(end)) return false;
        if (start == end) return true;

        std::vector<bool> visited(adjacencyList_.size(), false);
        std::queue<int> pending;
        visited[static_cast<std::size_t>(start)] = true;
        pending.push(start);

        while (!pending.empty()) {
            const int node = pending.front();
            pending.pop();

            for (const auto& edge : adjacencyList_[static_cast<std::size_t>(node)]) {
                if (edge.first == end) return true;
                if (!visited[static_cast<std::size_t>(edge.first)]) {
                    visited[static_cast<std::size_t>(edge.first)] = true;
                    pending.push(edge.first);
                }
            }
        }
        return false;
    }

    // Returns a path with the fewest edges (BFS), not the minimum total weight.
    // Returns an empty vector if the endpoints are invalid or disconnected.
    std::vector<int> getPath(int start, int end) const {
        if (!isValidVertex(start) || !isValidVertex(end)) return {};

        std::vector<int> parent(adjacencyList_.size(), -1);
        std::vector<bool> visited(adjacencyList_.size(), false);
        std::queue<int> pending;
        visited[static_cast<std::size_t>(start)] = true;
        pending.push(start);

        while (!pending.empty() && !visited[static_cast<std::size_t>(end)]) {
            const int node = pending.front();
            pending.pop();

            for (const auto& edge : adjacencyList_[static_cast<std::size_t>(node)]) {
                const int next = edge.first;
                if (!visited[static_cast<std::size_t>(next)]) {
                    visited[static_cast<std::size_t>(next)] = true;
                    parent[static_cast<std::size_t>(next)] = node;
                    pending.push(next);
                }
            }
        }

        if (!visited[static_cast<std::size_t>(end)]) return {};

        std::vector<int> path;
        for (int node = end; node != -1; node = parent[static_cast<std::size_t>(node)]) {
            path.push_back(node);
            if (node == start) break;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<int> bfsTraverse(int start) const {
        if (!isValidVertex(start)) return {};

        std::vector<int> result;
        std::vector<bool> visited(adjacencyList_.size(), false);
        std::queue<int> pending;
        visited[static_cast<std::size_t>(start)] = true;
        pending.push(start);

        while (!pending.empty()) {
            const int node = pending.front();
            pending.pop();
            result.push_back(node);

            for (const auto& edge : adjacencyList_[static_cast<std::size_t>(node)]) {
                const int next = edge.first;
                if (!visited[static_cast<std::size_t>(next)]) {
                    visited[static_cast<std::size_t>(next)] = true;
                    pending.push(next);
                }
            }
        }
        return result;
    }

    // Dijkstra requires every edge weight to be non-negative.
    // Returns -1 if the destination is unreachable or an endpoint is invalid.
    // Throws std::domain_error if any graph edge has a negative weight.
    Weight dijkstra(int start, int end) const {
        if (!isValidVertex(start) || !isValidVertex(end)) return static_cast<Weight>(-1);

        for (const auto& neighbours : adjacencyList_) {
            for (const auto& edge : neighbours) {
                if (edge.second < Weight{}) {
                    throw std::domain_error("Dijkstra cannot process negative edge weights.");
                }
            }
        }

        const Weight infinity = std::numeric_limits<Weight>::max();
        std::vector<Weight> distance(adjacencyList_.size(), infinity);
        using QueueEntry = std::pair<Weight, int>;
        std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> pending;

        distance[static_cast<std::size_t>(start)] = Weight{};
        pending.push({Weight{}, start});

        while (!pending.empty()) {
            const auto [currentCost, node] = pending.top();
            pending.pop();

            if (currentCost != distance[static_cast<std::size_t>(node)]) continue;
            if (node == end) return currentCost;

            for (const auto& edge : adjacencyList_[static_cast<std::size_t>(node)]) {
                const int next = edge.first;
                const Weight edgeWeight = edge.second;

                // Prevent signed overflow while adding non-negative values.
                if (currentCost > infinity - edgeWeight) continue;
                const Weight candidate = static_cast<Weight>(currentCost + edgeWeight);

                if (candidate < distance[static_cast<std::size_t>(next)]) {
                    distance[static_cast<std::size_t>(next)] = candidate;
                    pending.push({candidate, next});
                }
            }
        }
        return static_cast<Weight>(-1);
    }

    std::vector<ConnectedComponent<Weight>> connectedComponents() const {
        std::vector<ConnectedComponent<Weight>> components;
        std::vector<bool> visited(adjacencyList_.size(), false);

        for (int start = 0; start < vertexCount(); ++start) {
            if (visited[static_cast<std::size_t>(start)]) continue;

            components.push_back({});
            auto& component = components.back();
            component.id = static_cast<int>(components.size()) - 1;

            std::queue<int> pending;
            pending.push(start);
            visited[static_cast<std::size_t>(start)] = true;

            while (!pending.empty()) {
                const int node = pending.front();
                pending.pop();
                component.nodes.push_back(node);

                for (const auto& edge : adjacencyList_[static_cast<std::size_t>(node)]) {
                    if (!component.hasEdges) {
                        component.bitwiseAndCost = edge.second;
                        component.hasEdges = true;
                    } else {
                        component.bitwiseAndCost &= edge.second;
                    }

                    const int next = edge.first;
                    if (!visited[static_cast<std::size_t>(next)]) {
                        visited[static_cast<std::size_t>(next)] = true;
                        pending.push(next);
                    }
                }
            }
        }
        return components;
    }

    // Returns the bitwise AND of all edge weights in the shared connected
    // component, or -1 if the vertices are invalid or in different components.
    // A component with no edges has a cost of 0.
    Weight findMinimumBitwiseAndCost(int start, int end) const {
        if (!isValidVertex(start) || !isValidVertex(end)) return static_cast<Weight>(-1);

        const auto components = connectedComponents();
        for (const auto& component : components) {
            const bool containsStart =
                std::find(component.nodes.begin(), component.nodes.end(), start) != component.nodes.end();
            const bool containsEnd =
                std::find(component.nodes.begin(), component.nodes.end(), end) != component.nodes.end();
            if (containsStart && containsEnd) {
                return component.hasEdges ? component.bitwiseAndCost : Weight{};
            }
        }
        return static_cast<Weight>(-1);
    }

private:
    // Each entry is {neighbour vertex, edge weight}.
    std::vector<std::vector<std::pair<int, Weight>>> adjacencyList_;

    static std::size_t checkedVertexCount(int count) {
        if (count < 0) throw std::invalid_argument("Vertex count cannot be negative.");
        return static_cast<std::size_t>(count);
    }

    bool isValidVertex(int vertex) const noexcept {
        return vertex >= 0 && static_cast<std::size_t>(vertex) < adjacencyList_.size();
    }

    void validateVertex(int vertex) const {
        if (!isValidVertex(vertex)) throw std::out_of_range("Vertex ID is out of range.");
    }
};
