//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <logbench/logtest.hpp>

#include <thread>
#include <limits>
#include <cstdint>

#include <logbench/proc_high_prio.hpp>
#include <logbench/logger_thread.hpp>
#include <logbench/latch.hpp>
#include <logbench/force_inline.hpp>

namespace logbench {
   
    template<class logger>
    class logtest1 : public logtest<logger> {
    public:
        logtest1() : logtest<logger>("logtest1") {
            try {
                proc_high_prio();

                std::array<std::thread, 32> test_threads;
                const int thr_num = this->test_data_.thread_num;
                if (thr_num > 32) {
                    throw std::runtime_error("Thread number is max 32!");
                }
                latch start_latch(thr_num + 1);
                this->set_begin_logging_time();
                if (this->test_data_.save_latency) {
                    for (int thread_id = 0; thread_id < thr_num; thread_id++) {
                        auto temp{
                            std::thread(
                                logger_thread<logger, logtest1, true>,
                                thread_id,
                                std::ref(start_latch),
                                std::ref(this->bench_data_)
                            )
                        };
                        test_threads[thread_id].swap(temp);
                    }
                }
                else {
                    for (int thread_id = 0; thread_id < thr_num; thread_id++) {
                        auto temp{
                            std::thread(
                                logger_thread<logger, logtest1, false>,
                                thread_id,
                                std::ref(start_latch),
                                std::ref(this->bench_data_)
                            )
                        };
                        test_threads[thread_id].swap(temp);
                    }
                }

                start_latch.wait();
                for (int t = 0; t < thr_num; t++) {
                    test_threads[t].join();
                }
            }
            catch (const std::exception& ex) {
				this->out_data_.out_message = std::string_view{ ex.what() };
                this->bench_data_.set_data(this->out_data_);
                throw ex;
            }
        }

        LOGBENCH_FORCEINLINE static void log(
            logger& logger_,
            int id, 
			std::uint64_t i,
            [[maybe_unused]] std::uint64_t thr_dummy,
			std::uint64_t call_time)
        {
            logger_.log_test1(id, i, call_time, double(123.456789), std::numeric_limits<float>::infinity());
        }
    };
}
