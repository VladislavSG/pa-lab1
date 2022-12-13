#include <iostream>
#include <vector>
#include "seq_quicksort.h"
#include "par_quicksort.h"
#include <numeric>
#include <cassert>
#include <chrono>
#include <functional>
#include <oneapi/tbb.h>

std::size_t VEC_SIZE = 100000000ull;
unsigned int REPEAT_SORT = 5;
unsigned int P_FACTOR = 4;

template<typename Int>
void fill_random(std::vector<Int> &v) {
    static std::mt19937 gen(3);
    std::uniform_int_distribution<Int> random_int(
        std::numeric_limits<Int>::min(),
        std::numeric_limits<Int>::max()
    );
    for (Int &x : v) {
        x = random_int(gen);
    }
}

seq_quicksort<int32_t> SEQ(3);
par_quicksort<int32_t> PAR(3);

auto time(std::function<void()> const &f) {
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();
    f();
    auto end = clock::now();
    return end - start;
}

auto avg_sort_time(quicksort<int> const * const &alg) {
    std::chrono::duration<double> sum{};
    std::vector<int> vec(VEC_SIZE);
    for (unsigned int i = 0; i < REPEAT_SORT; ++i) {
        fill_random(vec);
        sum += time([&](){
            alg->sort(vec.begin(), vec.end());
        });
        assert(std::is_sorted(vec.begin(), vec.end()));
    }
    return sum / REPEAT_SORT;
}

void speed_test() {
    using namespace std::chrono;

    auto par_time = avg_sort_time(&PAR);
    std::cout  << "par = " << par_time.count() << std::endl;

    auto seq_time = avg_sort_time(&SEQ);
    std::cout << "seq = " << seq_time.count() << std::endl;

    double ratio = seq_time / par_time;
    std::cout << "Speedup ratio = " << ratio << "\n";
}

int main() {
    tbb::global_control limit_parallelize(tbb::global_control::max_allowed_parallelism, P_FACTOR);
    speed_test();
    return 0;
}
