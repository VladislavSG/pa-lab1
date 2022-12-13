#pragma once
#include "quicksort.h"
#include <random>
#include <cassert>

template <typename T>
class seq_quicksort : public quicksort<T> {
protected:
    using base = quicksort<T>;
    using it = base::it;

    it random_pivot(const it l, const it r) const {
        std::size_t dist = std::distance(l, r);
        std::uniform_int_distribution random_pos{0ul, dist};
        return l + random_pos(base::gen);
    }

    it partition(it l, it r, it const &pivot) const {
        auto pivot_value = *pivot;
        while (true) {
            while (*l < pivot_value) {
                ++l;
            }
            while (*r > pivot_value) {
                --r;
            }
            if (l >= r) {
                return r;
            }
            std::swap(*l++, *r--);
        }
    }

    it partition(it l, it r) const {
        return partition(l, r, random_pivot(l, r));
    }

    void sort_end_included(it const l, it const r) const {
        if (std::distance(l, r) < 1) return;
        //debug_print(l, r);
        it pivot = partition(l, r);
        sort_end_included(l, pivot);
        sort_end_included(pivot+1, r);
    }

public:
    explicit seq_quicksort(std::mt19937::result_type seed = std::random_device()())
        : base(seed) {};

    void sort(it const l, it const r) const override {
        if (l != r) {
            sort_end_included(l, r - 1);
        }
    }
};
