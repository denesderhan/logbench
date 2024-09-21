//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstddef>
#include <mutex>
#include <condition_variable>

namespace logbench {
    class latch {
    public:
        latch(std::size_t counter = 1)
            : counter_{ counter } {
        }
        
        inline void wait()
        {
            std::unique_lock<std::mutex> lock(mtx_);
            counter_--;
            if (counter_ == 0) {
                lock.unlock();
                cv_.notify_all();
                return;
            }
            else {
                while (counter_ != 0) {
                    cv_.wait(lock);
                }
            }
        }

    private:
        std::mutex mtx_;
        std::condition_variable cv_;
		std::size_t counter_;
    };
}
