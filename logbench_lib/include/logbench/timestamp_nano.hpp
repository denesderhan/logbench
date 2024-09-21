//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <chrono>
#include <cstdint>

#include <logbench/force_inline.hpp>

namespace logbench {
	template<bool sys_clock = false>
	LOGBENCH_FORCEINLINE std::uint64_t timestamp_nano() noexcept {
		std::uint64_t out{ 0 };
		if constexpr (sys_clock) {
			constexpr std::uint64_t den{ std::chrono::system_clock::time_point::period::den };
			constexpr std::uint64_t num{ std::chrono::system_clock::time_point::period::num };
			const auto time_point = std::chrono::system_clock::now();
			std::uint64_t out = time_point.time_since_epoch().count();
			if constexpr (den <= 1'000'000'000ULL) {
				out *= (1'000'000'000ULL / den) * num;
			}
			else {
				out *= num * 1'000'000'000ULL;
				out /= den;
			}
			return out;
		}
		else {
			constexpr std::uint64_t den{ std::chrono::steady_clock::time_point::period::den };
			constexpr std::uint64_t num{ std::chrono::steady_clock::time_point::period::num };
			const auto time_point = std::chrono::steady_clock::now();
			std::uint64_t out = time_point.time_since_epoch().count();
			if constexpr (den <= 1'000'000'000ULL) {
				out *= (1'000'000'000ULL / den) * num;
			}
			else {
				out *= num * 1'000'000'000ULL;
				out /= den;
			}
			return out;
		}
	}
}
