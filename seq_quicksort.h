#pragma once
#include "quicksort.h"
#include <random>
#include <cassert>

template <typename T>
class seq_quicksort : public quicksort<T> {
public:
    using base = quicksort<T>;
    using it = base::it;

    it random_pivot(const it l, const it r) const {
        std::size_t dist = std::distance(l, r);
        std::uniform_int_distribution random_pos{0ul, dist-1};
        return l + random_pos(base::gen);
    }

    static it partition(it l, it r, it pivot) {
        if(pivot != l) {
            std::iter_swap(l, pivot);
            pivot = l;
        }

        for(;;) {
            do {
                --r;
            } while(*pivot < *r);
            do {
                if( l >= r ) break;
                ++l;
            } while(*l < *pivot);
            if( l >= r ) break;
            std::iter_swap(l, r);
        }

        std::iter_swap(pivot, r);
        return r;
    }

    it partition(it l, it r) const {
        return partition(l, r, random_pivot(l, r));
    }

    explicit seq_quicksort(std::mt19937::result_type seed = std::random_device()())
        : base(seed) {};

    inline void sort(it const l, it const r) const override {
        if (std::distance(l, r) <= 1) return;
        it pivot = partition(l, r);
        sort(l, pivot);
        sort(pivot+1, r);
    }
};
