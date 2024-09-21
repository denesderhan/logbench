//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <memory>
#include <cstdint>

#include <logbench/api_def.hpp>

namespace logbench {
	class precision_timer_impl;
	class precision_timer {
	public:
		LOGBENCH_API precision_timer();
		LOGBENCH_API ~precision_timer();
		LOGBENCH_API void sleep(std::uint64_t nanosec);
	private:
		std::unique_ptr<precision_timer_impl> timer_;
	};
}
