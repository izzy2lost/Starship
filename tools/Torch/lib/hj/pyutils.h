#pragma once

#include <vector>
#include <unordered_map>

namespace PyUtils {
template<typename T>
std::vector<T> slice(std::vector<T> const &v, uint32_t m, uint32_t n = -1) {
    auto first = v.cbegin() + m;
    auto last = n == -1 ? v.cend() : v.cbegin() + n;

    std::vector<T> vec(first, last);
    return vec;
}

std::vector<uint32_t> range(uint32_t start, uint32_t end);

template<typename T>
T max(std::vector<T> const &v) {
    T max = v[0];
    for (auto const &value: v) {
        if (value > max) {
            max = value;
        }
    }
    return max;
}
template<typename T>
T min(std::vector<T> const &v) {
    T min = v[0];
    for (auto const &value: v) {
        if (value < min) {
            min = value;
        }
    }
    return min;
}

template<typename T, typename X>
std::vector<T> keys(std::unordered_map<T, X> const &map) {
    std::vector<T> result;
    for (auto const &pair: map) {
        result.push_back(pair.first);
    }
    return result;
}
template<typename T, typename X>
std::vector<T> keys(std::map<T, X> const &map) {
    std::vector<T> result;
    for (auto const &pair: map) {
        result.push_back(pair.first);
    }
    return result;
}
}

template <typename T>
std::vector<T> operator+(std::vector<T> const &x, std::vector<T> const &y)
{
    std::vector<T> vec;
    vec.reserve(x.size() + y.size());
    vec.insert(vec.end(), x.begin(), x.end());
    vec.insert(vec.end(), y.begin(), y.end());
    return vec;
}