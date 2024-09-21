//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <logbench/api_def.hpp>

namespace logbench {
	// During the lifetime of an instance keeps the machine in high power state and disables sleep.
	// Not thread safe! Only one instance allowed in a process!
	class proc_max_power {
	public:
		LOGBENCH_API proc_max_power() noexcept;
		LOGBENCH_API ~proc_max_power() noexcept;
	};
}
