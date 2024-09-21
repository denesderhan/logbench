//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>
#pragma intrinsic(memcpy)

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
		auto logger_ = logger(thread_id);
		precision_timer p_timer;

		[[maybe_unused]] char* latency_ptr = b_data.latency_data(thread_id);

		pin_thread::pin(thread_id);

		test_in_param in_data;
		b_data.get_data(in_data);
		if (in_data.set_prio_high) {
			thread_high_prio();
		}

		test_result_thread data;
		auto log_num_target = in_data.log_call_per_thread;
		const std::uint64_t test_dur_targ = static_cast<std::uint64_t>(in_data.test_duration_target) * 1'000'000;
		const auto work = in_data.work_cycles;
		std::uint64_t sum_latency{ 0 };
		start_latch.wait();
		const std::uint64_t start_time = timestamp_nano();
		std::uint64_t thr_dummy = start_time;
		std::uint64_t log_num_in_avg{ 0 };
		unsigned int log_num{ 0 };
		while(log_num != log_num_target){
			log_num++;
			thr_dummy = do_work(work, thr_dummy);
			std::uint32_t id1;
			std::uint32_t id2;
			const std::uint64_t latency_start = timestamp_tsc(id1);
			test<logger>::log(logger_, thread_id, log_num, thr_dummy, latency_start);
			const std::uint64_t latency_stop = timestamp_tsc(id2);

			if constexpr (with_latencies) {
				memcpy(
					latency_ptr,
					&latency_start,
					sizeof(std::uint64_t)
				);
				latency_ptr += sizeof(std::uint64_t);
				memcpy(
					latency_ptr,
					&latency_stop,
					sizeof(std::uint64_t)
				);
				latency_ptr += sizeof(std::uint64_t);
				memcpy(
					latency_ptr,
					&id1,
					sizeof(std::uint32_t)
				);
				latency_ptr += sizeof(std::uint32_t);
				memcpy(
					latency_ptr,
					&id2,
					sizeof(std::uint32_t)
				);
				latency_ptr += sizeof(std::uint32_t);
			}
			int use_log{
				(id1 == id2)
				& (latency_stop > latency_start)
			};
			log_num_in_avg += use_log;
			sum_latency += (latency_stop - latency_start) * use_log;
			const auto curr_dur{ timestamp_nano() - start_time };
			// max test duration: 3'600'000'000'000 max log num: 1'000'000 is less than UINT64MAX
			const std::uint64_t curr_dur_targ = (test_dur_targ * log_num) / log_num_target;
			if (curr_dur < curr_dur_targ) {
				p_timer.sleep(curr_dur_targ - curr_dur);
			}
		}

		if (log_num_in_avg == 0) {
			sum_latency = 0;
		}
		else if(log_num_in_avg != log_num){
			// compensating dismissed latency measurements
			sum_latency = sum_latency + (sum_latency / log_num_in_avg) * (log_num - log_num_in_avg);
		}
		data.sum_latency = sum_latency;
		data.test_start = start_time;
		data.log_num = log_num;
		data.work_dummy = thr_dummy;
		const auto sum_time{ timestamp_nano() - start_time };
		if (sum_time < test_dur_targ) {
			p_timer.sleep(test_dur_targ - sum_time);
		}
		data.test_stop = timestamp_nano();
		b_data.set_data(thread_id, data);
	}
}
