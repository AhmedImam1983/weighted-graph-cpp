#pragma once

#include <cstddef>
#include <vector>

// For each index, returns the distance to the next strictly greater value
// on its right; returns 0 when no such value exists.
inline std::vector<int> distanceToNextGreater(const std::vector<int>& values) {
    std::vector<int> result(values.size(), 0);
    std::vector<std::size_t> indices;
    indices.reserve(values.size());

    for (std::size_t i = 0; i < values.size(); ++i) {
        while (!indices.empty() && values[i] > values[indices.back()]) {
            const std::size_t previous = indices.back();
            indices.pop_back();
            result[previous] = static_cast<int>(i - previous);
        }
        indices.push_back(i);
    }
    return result;
}

// For each value, returns the next strictly greater value on its right,
// or -1 when no such value exists.
inline std::vector<int> nextGreaterElements(const std::vector<int>& values) {
    std::vector<int> result(values.size(), -1);
    std::vector<std::size_t> indices;
    indices.reserve(values.size());

    for (std::size_t i = 0; i < values.size(); ++i) {
        while (!indices.empty() && values[i] > values[indices.back()]) {
            result[indices.back()] = values[i];
            indices.pop_back();
        }
        indices.push_back(i);
    }
    return result;
}
