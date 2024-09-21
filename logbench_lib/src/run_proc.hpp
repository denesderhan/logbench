//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <string>
#include <logbench/proc_run_stats.hpp>

namespace logbench {
	void run_proc(std::string const& path, proc_run_stats& stats) noexcept;
}
