//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <system_error>
#include <time.h>
#include <cstdint>

namespace logbench {
	class precision_timer_impl {
	public:
		precision_timer_impl() {}
		~precision_timer_impl() {}
		static void sleep(std::uint64_t nanosec) {
			timespec duration;
			duration.tv_sec = nanosec / 1'000'000'000LL;
			duration.tv_nsec = nanosec - (1'000'000'000LL * duration.tv_sec);
			nanosleep(&duration, NULL);
		}
	};
}


