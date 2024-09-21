//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <logbench/precision_timer.hpp>

#include <cstdint>

#if defined(_WIN32)
#include <precision_timer_impl_win.hpp>
#else
#include <precision_timer_impl_posix.hpp>
#endif

namespace logbench {
	precision_timer::precision_timer()
		: timer_{ std::move(std::make_unique<precision_timer_impl>()) } {
	}
	precision_timer::~precision_timer() = default;
	
	void precision_timer::sleep(std::uint64_t nanosec) {
		timer_->sleep(nanosec);
	}
};




