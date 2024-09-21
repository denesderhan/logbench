//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <powersetting.h>
#pragma comment(lib, "powrprof.lib")

namespace logbench {
    class max_power_impl {
    public:
		max_power_impl() = default;
		~max_power_impl() = default;
		//non thread safe
		static void set() noexcept {
			if (guid_power_scheme_ == nullptr) {
				DWORD error = PowerGetActiveScheme(0, &guid_power_scheme_);
				if (!error) {
					//If the function fails, the return value is NULL
					const auto result = SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
					if (result == NULL) {
						error = 1;
					}
				}
				if (!error) {
					//Returns ERROR_SUCCESS (zero) if the call was successful, and a nonzero value if the call failed.
					error = PowerSetActiveScheme(0, &GUID_MIN_POWER_SAVINGS);
				}
				if (error) {
					guid_power_scheme_ = nullptr;
				}
			}
		}

		//non thread safe
		static void reset() noexcept {
			if (guid_power_scheme_ != nullptr) {
				PowerSetActiveScheme(0, guid_power_scheme_);
				SetThreadExecutionState(ES_CONTINUOUS);
			}
		}

    private:
		inline static GUID* guid_power_scheme_ { nullptr };
    };
}
