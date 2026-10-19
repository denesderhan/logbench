//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>

namespace logbench {
    struct alignas(64) test_result_thread {
        std::uint64_t test_start{ 0 }; // nano time
        std::uint64_t test_stop{ 0 }; // nano time
        std::uint64_t avg_latency{ 0 }; // average latency in TSC cycles (including TSC call overhead)
        std::uint64_t latency_samples{ 0 }; // number of latency samples used for average
        std::uint64_t log_num{ 0 }; // number of log calls
        std::uint64_t work_dummy{ 0 }; // dummy value to avoid compiler optimizing away the work
        char padding[16]{ 0 };
    };
    static_assert(sizeof(test_result_thread) == 64, "Bad");
}
