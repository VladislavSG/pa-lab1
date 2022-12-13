#pragma once

#include <vector>
#include <random>

template<typename T>
class quicksort {
protected:
    using it = std::vector<T>::iterator;
    mutable std::mt19937 gen;

    template<typename It>
    static void debug_print(It l, It r) {
        while (l != r) {
            std::cout << *l++ << ' ';
        }
        std::cout << std::endl;
    }
public:
    explicit quicksort(std::mt19937::result_type seed) : gen(seed) {}

    virtual void sort(it l, it r) const = 0;
};
