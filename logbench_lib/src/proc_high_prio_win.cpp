//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#if defined(_WIN32)

#include <logbench/proc_high_prio.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <system_error>

namespace logbench {
	void proc_high_prio() {
		if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS)) {
			throw std::system_error{
				std::error_code{
					static_cast<int>(GetLastError()),
						std::system_category()
				},
					"Could not set process priority to high!"
			};
		}
	}
}
#endif
