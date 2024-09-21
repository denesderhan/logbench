//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#if defined(_WIN32)

#include <logbench/thread_high_prio.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <system_error>

namespace logbench {
	void thread_high_prio() {
		if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST)) {
			throw std::system_error{
				std::error_code{
					static_cast<int>(GetLastError()),
						std::system_category()
				},
					"Could not set thread priority to highest!"
			};
		}
	}
}
#endif
