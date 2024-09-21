//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <system_error>
#include <cstdint>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Timeapi.h>
#pragma comment(lib, "Winmm.lib") // timeGetDevCaps, timeBeginPeriod

namespace logbench {
	class precision_timer_impl {
	public:
		precision_timer_impl() {
			//Starting with Windows 10, version 2004, this function no longer affects global timer resolution.
			timeBeginPeriod(min_timer_period_milli());

			timer_ = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
			if (timer_ == nullptr) {
				throw std::system_error{ 
					std::error_code{
						static_cast<int>(GetLastError()), 
						std::system_category()
					}, 
					"Could not create precision timer object!"
				};
			}
		}
		~precision_timer_impl() {
			timeEndPeriod(min_timer_period_milli());
			CloseHandle(timer_);
		}
		void sleep(std::uint64_t nanosec) {
			std::uint64_t qpc_to_sleep = (nanosec / 1000 ) * qpc_per_micro_;
			if (qpc_to_sleep >= scheduler_period_qpc_) {
				//Negative values indicate relative time.
				LARGE_INTEGER neg_qpc_to_sleep;
				neg_qpc_to_sleep.QuadPart = -static_cast<std::int64_t>(qpc_to_sleep);
				if (!SetWaitableTimerEx(timer_, &neg_qpc_to_sleep, 0, NULL, NULL, NULL, 0)) {
					throw std::system_error{
						std::error_code{
							static_cast<int>(GetLastError()),
								std::system_category()
						},
						"Could not set precision timer!"
					};
				}
				WaitForSingleObject(timer_, INFINITE);
			}
		}

		static std::uint64_t qpc_per_sec() noexcept {
			static std::uint64_t qpc_sec = []()-> std::uint64_t {
				LARGE_INTEGER qpf;
				QueryPerformanceFrequency(&qpf);
				if (qpf.QuadPart == 0) {
					throw std::system_error{
						std::error_code{
							static_cast<int>(GetLastError()),
							std::system_category()
						},
						"Could not get QueryPerformanceFrequency!"
					};
				}
				return static_cast<std::uint64_t>(qpf.QuadPart);
			}();
			return qpc_sec;
		}

		static UINT min_timer_period_milli() {
			static UINT t_per = []()-> UINT {
				TIMECAPS caps;
				auto err = timeGetDevCaps(&caps, sizeof caps);
				if (err) {
					throw std::system_error{
						std::error_code{
							static_cast<int>(err),
							std::system_category()
						},
						"Could not get timer min period value!"
					};
				}
				return caps.wPeriodMin;
			}();
			return t_per;
		}

		static std::uint64_t sched_period_qpc() {
			return static_cast<std::uint64_t>(min_timer_period_milli()) * (qpc_per_sec() / 1000);
		}

		HANDLE timer_;
		static inline std::uint64_t scheduler_period_qpc_{ sched_period_qpc() };
		static inline std::uint64_t qpc_per_micro_{ qpc_per_sec() / 1'000'000 };
	};
}


