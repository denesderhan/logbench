//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>
#include <cstring>

#include <logbench/bench_data.hpp>
#include <logbench/precision_timer.hpp>
#include <logbench/do_work.hpp>
#include <logbench/force_inline.hpp>
#include <logbench/latch.hpp>
#include <logbench/pin_thread.hpp>
#include <logbench/test_result_thread.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/thread_high_prio.hpp>
#include <logbench/timestamp_nano.hpp>
#include <logbench/timestamp_tsc.hpp>

namespace logbench {
    template<class logger, template <class> class test, bool with_latencies >
    LOGBENCH_FORCEINLINE void logger_thread(int thread_id, latch& start_latch, bench_data& b_data)
    {
        logger logger_{ thread_id };
        precision_timer p_timer;

        [[maybe_unused]] char* latency_ptr = b_data.latency_data(thread_id);
        
        pin_thread::pin(thread_id);

        test_in_param in_data;
        b_data.get_data(in_data);
        if (in_data.set_prio_high) {
            thread_high_prio();
        }
        // input in milliseconds, convert to nanoseconds
        const std::uint64_t duration_target = 
            static_cast<std::uint64_t>(in_data.test_duration_target) * 1'000'000;
        const std::uint64_t log_num_target = in_data.log_call_per_thread;
        // 1 cycle duration target in nanoseconds, may be 0 on purpose to not throttle the logging calls, 
        // and the test duration will be longer than requested
        const std::uint64_t cycle_time = log_num_target == 0 ? 0 : duration_target / log_num_target;
        const std::uint64_t work = in_data.work_cycles;
        std::uint64_t sum_latency{ 0 };
        std::uint64_t latencies_used{ 0 };
        std::uint64_t log_num{ 0 };
        start_latch.wait();
        const std::uint64_t start_time = timestamp_nano();
        std::uint64_t thr_dummy = start_time;
        std::uint64_t expected_cycle_stop = start_time;
        while (log_num != log_num_target) {
            log_num++;
            expected_cycle_stop += cycle_time;
            thr_dummy = do_work(work, thr_dummy);
                        
            latency_record latency_rec;
            latency_rec.nano_time = timestamp_nano();
            latency_rec.tsc_start = timestamp_tsc(latency_rec.cpu_1);
            test<logger>::log(logger_, thread_id, log_num, thr_dummy, latency_rec.nano_time);
            latency_rec.tsc_stop = timestamp_tsc(latency_rec.cpu_2);

            if constexpr (with_latencies) {                
                memcpy(
                    latency_ptr,
                    &latency_rec,
                    sizeof(latency_rec)
                );
                latency_ptr += sizeof(latency_rec);
            }
            
            // branchless accumulation of valid latency samples (same CPU and stop >= start)
            std::uint64_t valid_latency{
                std::uint64_t(latency_rec.cpu_1 == latency_rec.cpu_2)
                & std::uint64_t(latency_rec.tsc_stop >= latency_rec.tsc_start)
            };
            latencies_used += valid_latency;
            sum_latency += (latency_rec.tsc_stop - latency_rec.tsc_start) * valid_latency;
            
            std::uint64_t cycle_stop{ timestamp_nano() };
            // input parameters are constrained to max test duration: 3'600'000'000'000 and max log num: 1'000'000. (less than UINT64MAX)
            if (cycle_stop < expected_cycle_stop) {
                p_timer.sleep(expected_cycle_stop - cycle_stop);
            }
        }
        const auto test_duration{ timestamp_nano() - start_time };
        if (test_duration < duration_target) {
            p_timer.sleep(duration_target - test_duration);
        }

        test_result_thread data{};
        if (latencies_used == 0) {
            data.avg_latency = 0;
        }
        else {
            data.avg_latency = sum_latency / latencies_used;
        }

        data.latency_samples = latencies_used;
        data.test_start = start_time;
        data.log_num = log_num;
        data.work_dummy = thr_dummy;
        data.test_stop = timestamp_nano();
        b_data.set_thread_data(thread_id, data);
    }
}
