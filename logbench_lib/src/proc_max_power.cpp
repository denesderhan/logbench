//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <logbench/proc_max_power.hpp>

#if defined(_WIN32)
#include <max_power_impl_win.hpp>
#else
#include <max_power_impl_posix.hpp>
#endif

namespace logbench {
	proc_max_power::proc_max_power() noexcept {
		max_power_impl::set();
	}
	proc_max_power::~proc_max_power() noexcept {
		max_power_impl::reset();
	}
}
