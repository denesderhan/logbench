//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>
#include <logbench/small_string.hpp> 

namespace logbench {
	struct proc_run_stats {
		small_string<128> error;
		//nanoseconds
		std::uint64_t process_start_time{ 0 };
		std::uint64_t process_stop_time{ 0 };
		//windows linux is it the same????
		std::uint64_t cpu_cycles{ 0 };
		std::uint64_t page_faults{ 0 };
		//in byte
		std::uint64_t peak_memory{ 0 };
		std::uint64_t exit_code{ 0 };
		char padding[80]{ 0 };
	};
	static_assert(sizeof(proc_run_stats) == 256, "Bad");
}
