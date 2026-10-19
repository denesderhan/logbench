//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#ifndef _WIN32
#include <logbench/pin_thread.hpp>

#include <cerrno>
#include <sched.h>
#include <system_error>

namespace logbench {
    void pin_thread::pin(int thread_number) {
        int core_num = thread_to_core_[thread_number % thread_to_core_.size()];
        if (core_num >= 0) {
            cpu_set_t mask;
            CPU_ZERO(&mask);
            CPU_SET(core_num, &mask);

            if (sched_setaffinity(0, sizeof(mask), &mask) != 0) {
                throw std::system_error(errno, std::generic_category(), "Could not pin thread to core!");
            }
        }
    }
}
#endif
