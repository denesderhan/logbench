//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <logbench/logtest.hpp>

#include <cinttypes>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <thread>
#include <vector>

#include <logbench/latch.hpp>
#include <logbench/logger_thread.hpp>
#include <logbench/proc_high_prio.hpp>

namespace logbench {
   
    template<class logger>
    class logtest1 : public logtest<logger> {
    public:
        logtest1() : logtest<logger>("logtest1") {
            try {
                proc_high_prio();

                std::vector<std::thread> test_threads;
                const int thr_num = this->test_data_.thread_num;
                latch start_latch(thr_num + 1);
                this->set_begin_logging_time();
                if (this->test_data_.save_latency) {
                    for (int thread_id = 0; thread_id < thr_num; thread_id++) {
                        test_threads.emplace_back(
                            std::thread(
                                logger_thread<logger, logtest1, true>,
                                thread_id,
                                std::ref(start_latch),
                                std::ref(this->bench_data_)));
                    }
                }
                else {
                    for (int thread_id = 0; thread_id < thr_num; thread_id++) {
                        test_threads.emplace_back(
                            std::thread(
                                logger_thread<logger, logtest1, false>,
                                thread_id,
                                std::ref(start_latch),
                                std::ref(this->bench_data_)));
                    }
                }

                start_latch.wait();
                for (auto& t : test_threads) t.join();
            }
            catch (const std::exception& ex) {
                this->out_data_.out_message = std::string_view{ ex.what() };
                this->bench_data_.set_data(this->out_data_);
                throw ex;
            }
        }

        static void log(
            logger& logger_,
            int id,
            std::uint64_t i,
            [[maybe_unused]] std::uint64_t thr_dummy,
            std::uint64_t call_time)
        {
#if defined LOGBENCH_LOGCALL_FMT
            LOGBENCH_LOG_INFO(logger_, "Thr: {} Log_n: {} Time: {} {} {}", id, i, call_time, \
                double(123.456789), std::numeric_limits<float>::infinity());
#elif defined LOGBENCH_LOGCALL_PRINTF
            LOGBENCH_LOG_INFO(logger_, "Thr: %d Log_n: %" PRIu64 " Time: %" PRIu64 " %f %f", \
                id, i, call_time, double(123.456789), std::numeric_limits<float>::infinity());
#elif defined LOGBENCH_LOGCALL_PRINTF_CUSTOM
            LOGBENCH_LOG_INFO(logger_, "Thr: " CUSTOM_FMT_INT32 " Log_n: " CUSTOM_FMT_UINT64 \
                " Time: " CUSTOM_FMT_UINT64 " " CUSTOM_FMT_DOUBLE " " CUSTOM_FMT_FLOAT, \
                id, i, call_time, double(123.456789), std::numeric_limits<float>::infinity());
#elif defined LOGBENCH_LOGCALL_STREAM
            LOGBENCH_LOG_INFO(logger_) << "Thr: " << id << " Log_n: " << i << " Time: " \
                << call_time << " " << double(123.456789) << " " << std::numeric_limits<float>::infinity();
#else
#error "The logcall format of the logging library is not supported!"
#endif
        }
    };
}
