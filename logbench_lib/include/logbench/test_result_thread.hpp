//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>

namespace logbench {
	struct test_result_thread {
		std::uint64_t test_start{ 0 };
		std::uint64_t test_stop{ 0 };
		std::uint64_t sum_latency{ 0 };
		std::uint64_t log_num{ 0 };
		std::uint64_t work_dummy{ 0 };
	};
	static_assert(sizeof(test_result_thread) == 40, "Bad");
}
