#pragma once

#include "quicksort.h"
#include "seq_quicksort.h"
#include <oneapi/tbb.h>

template<typename T>
class par_quicksort : public seq_quicksort<T> {
    using base = seq_quicksort<T>;
    using it = base::it;

    class sort_range {
        seq_quicksort<T> const *p;

        std::size_t split_range(sort_range &range) {
            it m = p->partition(range.begin, range.begin + range.size);
            std::size_t old_size = range.size;
            range.size = m - range.begin;
            return old_size - range.size - 1;
        }

    public:
        sort_range(const sort_range &) = default;
        void operator=(const sort_range &) = delete;

        static std::size_t constexpr BLOCK_SIZE = 0x200;
        std::size_t size{};
        it begin;

        sort_range(it begin, std::size_t size, seq_quicksort<T> const *p = nullptr)
                : p(p)
                , size(size)
                , begin(begin) {}

        [[nodiscard]] bool empty() const { return size == 0; }

        [[nodiscard]] bool is_divisible() const { return size >= BLOCK_SIZE; }

        sort_range(sort_range &range, tbb::split)
                : p(range.p)
                , size(split_range(range))
                , begin(range.begin + range.size + 1) {}
    };

public:
    explicit par_quicksort(std::mt19937::result_type seed = std::random_device()())
            : base(seed) {};

    void sort(it l, it r) const override {
        tbb::parallel_for(sort_range(l, r - l, this),
                          [&](sort_range const &r) {
                              std::sort(r.begin, r.begin + r.size);
                          },
                          tbb::auto_partitioner());
    }
};
