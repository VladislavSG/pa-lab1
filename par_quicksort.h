#pragma once

#include "quicksort.h"
#include "seq_quicksort.h"
#include <oneapi/tbb.h>

template<typename T>
class par_quicksort : public seq_quicksort<T> {
    using base = seq_quicksort<T>;
    using it = base::it;
    unsigned int const p_factor = 4;
    static std::size_t const BLOCK_SIZE = 0x200;

    std::vector<T> filter(it l, it r, std::function<bool(T const &)> const &f) const {
        using namespace tbb;
//        using vec = std::vector<int>;
//        return parallel_reduce(
//                blocked_range<it>(l, r),
//                vec{},
//                [&](const blocked_range<it>& r, vec const &v) {
//                    vec res(v);
//                    for(it a = r.begin(); a != r.end(); ++a )
//                        if (f(*a)) {
//                            res.emplace_back(*a);
//                        }
//                    return res;
//                },
//                [](vec const &x, vec const &y) {
//                    vec z;
//                    z.reserve(x.size() + y.size());
//                    z.insert(z.end(), x.begin(), x.end());
//                    z.insert(z.end(), y.begin(), y.end());
//                    return z;
//                },
//                static_partitioner()
//        );
        using pii = std::pair<it, it>;

        std::size_t const DIST = std::distance(l, r);
        std::size_t const STEP = DIST / p_factor;
        unsigned int const OST = DIST % p_factor;
        unsigned int group = 0;
        std::vector<T> result;

        parallel_pipeline(p_factor,
                          make_filter<void, pii>(
                                  filter_mode::serial_in_order,
                                  [&](tbb::flow_control &fc) -> pii {
                                      if (group >= p_factor) {
                                          fc.stop();
                                          return {r, r};
                                      }
                                      it ll = l;
                                      l += STEP;
                                      if (group++ < OST) {
                                          ++l;
                                      }
                                      return {ll, l};
                                  }
                          ) &
                          make_filter<pii, std::vector<int>>(
                                  filter_mode::parallel,
                                  [&](pii p) {
                                      std::vector<int> t_res;
                                      for (auto it = p.first; it != p.second; ++it) {
                                          if (f(*it)) {
                                              t_res.emplace_back(*it);
                                          }
                                      }
                                      return t_res;
                                  }
                          ) &
                          make_filter<std::vector<int>, void>(
                                  filter_mode::serial_out_of_order,
                                  [&](std::vector<int> const &x) {
                                        result.insert(result.end(), x.begin(), x.end());
                                  }
                          )
        );
        return result;
    }

    void sort_impl(it const &l, it const &r) const {
        if (std::distance(l, r) < BLOCK_SIZE) {
            base::sort(l, r);
        } else {
            auto ll = l, rr = r;
            {
                T const pivot = *base::random_pivot(l, r);

                auto smaller = filter(l, r, [pivot](T const &x) { return x < pivot; });
                auto bigger = filter(l, r, [pivot](T const &x) { return x > pivot; });

                for (auto i = smaller.begin(); i != smaller.end(); ++i) {
                    *ll++ = *i;
                }
                for (auto i = bigger.rbegin(); i != bigger.rend(); ++i) {
                    *--rr = *i;
                }
                std::fill(ll, rr, pivot);
            }
            tbb::parallel_invoke(
                    [&]() { sort_impl(l, ll); },
                    [&]() { sort_impl(rr, r); });
        }
    }

public:
    explicit par_quicksort(unsigned int p_factor, std::mt19937::result_type seed = std::random_device()())
            : p_factor(p_factor), base(seed) {};

    void sort(it l, it r) const override {
        sort_impl(l, r);
    }
};
