//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>
#include <logbench/small_string.hpp>
#include <logbench/formatter_typ.hpp>
#include <logbench/sink_typ.hpp>

namespace logbench {
	struct test_out_param {
		small_string<256> out_message;
		small_string<64> lib_name{};
		small_string<64> lib_version{};
		small_string<64> test_name{};
		small_string<64> formater_impl;
		//in nano seconds (excluding system setup)
		std::uint64_t logging_begin_time{ 0 };
		double rdtsc_freq{ 0.0 };
		unsigned int garbage_lines{ 0 };
		//-1 means unknown
		int lib_buffer_size{ -1 };
		//milliseconds
		int buffer_flush_time{ -1 };
		int sink_flush_time{ -1 };
		//lib means unknown
		formatter_typ formatter_type{ formatter_typ::txt };
		sink_typ sink_type{ sink_typ::lib };
		bool debug_build{ false };
		char padding[221]{ 0 };
	};
	static_assert(sizeof(test_out_param) == 768,"Bad");
}
