//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>
#include <string>

#include <logbench/small_string.hpp>
#include <logbench/fnv_hash.hpp>
#include <logbench/to_hexstring.hpp>
#include <logbench/formatter_typ.hpp>
#include <logbench/sink_typ.hpp>
#include <logbench/version.hpp>

namespace logbench {
	struct test_in_param {
		small_string<256> out_path{};
		small_string<256> temp_path{};
		small_string<256> log_path{};
		small_string<64> bench_version{LOGBENCHLIB_VERSION};
		//work done before a log call
		std::uint64_t work_cycles{ 0 };
		//targeted duration of test run
		unsigned int test_duration_target{ 0 };
		unsigned int thread_num{ 1 };
		unsigned int log_call_per_thread{ 10 };
		//Kbytes per thread -1 means use logging library default
		int lib_buffer_size{ -1 };
		//milliseconds -1 means use logging library default
		int buffer_flush_time{ -1 };
		//milliseconds -1 means use logging library default
		int sink_flush_time{ -1 };
		//negative means no pinning
		std::pair<int, int> pin_to_cores {-1, -1};
		bool log_self{ false };
		// set logger thread priority to high?
		bool set_prio_high{ true };
		
		formatter_typ formatter_type{ formatter_typ::txt };
		sink_typ sink_type{ sink_typ::lib };

		bool save_latency{ false };
		bool count_dropped{ false };
		char padding[146]{ 0 };
	};

	static_assert(sizeof(test_in_param) == 1024, "Bad" );
}
