#pragma once
#include <algorithm>
#include <any>
#include <iostream>
#include <vector>
#include <tuple>

template<class TupType, size_t... I>
void print_tuple(const TupType& _tup, std::index_sequence<I...>)
{
    std::cout << "(";
    (..., (std::cout << (I == 0? "" : ", ") << std::get<I>(_tup)));
    std::cout << ")\n";
}

template<class... T>
void print_tuple(const std::tuple<T...>& _tup)
{
    print_tuple(_tup, std::make_index_sequence<sizeof...(T)>());
}

template<typename T>
void variatic_min(std::vector<T> v, int &m) {
    if (m > v.size()) m = v.size();
}

template<int N, typename... Ts>
using NthTypeOf = typename std::tuple_element<N, std::tuple<Ts...>>::type;

template <typename... Result, std::size_t... Indices>
auto vec_to_tup_helper(const std::vector<std::any>& v, std::index_sequence<Indices...>) {
    return std::make_tuple(
	    std::any_cast<NthTypeOf<Indices, Result...>>(v[Indices])...
    );
}

template <typename ...Result>
std::tuple<Result...> vec_to_tup(std::vector<std::any>& values) {
    return vec_to_tup_helper<Result...>(values, std::make_index_sequence<sizeof...(Result)>());
}

template <typename T>
void celem_at(std::vector<T> v, int i, std::vector<std::any> &r) {
    r.push_back(v[i]);
}

template<typename... Types>
std::vector<std::tuple<Types...>> zip(std::vector<Types>... args) {
    int m = 0xFFFFFFFF;
    std::vector<std::tuple<Types...>> res;

    (variatic_min(args, m), ...);

    for (int i = 0; i < m; i++) {
        std::vector<std::any> vaux;
        (celem_at(args, i, vaux), ...);
        res.push_back(vec_to_tup<Types...>(vaux));
    }
    return res;
}