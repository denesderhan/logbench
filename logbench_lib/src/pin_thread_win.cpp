//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#if defined(_WIN32)

#include <logbench/pin_thread.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <system_error>

namespace logbench {
	void pin_thread::pin(int thread_number) {
		int core_num = thread_to_core_[thread_number % thread_to_core_.size()];
		if (core_num >= 0) {
			if (!SetThreadAffinityMask(GetCurrentThread(), DWORD_PTR(1) << core_num))
			{
				throw std::system_error{
					std::error_code{
						static_cast<int>(GetLastError()),
							std::system_category()
					},
						"Could not pin thread to core!"
				};
			}
		}
	}
}
#endif
