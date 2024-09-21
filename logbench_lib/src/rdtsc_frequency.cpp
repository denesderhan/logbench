//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <cstdint>
#include <cstdlib>

#include <logbench/timestamp_tsc.hpp>
#include <logbench/timestamp_nano.hpp>

namespace logbench {
	static double measure_freq() {
		const std::uint64_t measure_stop{ timestamp_nano() + 2'000'000'000ULL };
		double freq[100]{ 0.0 };
		bool freq_filled{ false };
		int freq_ind = 0;
		while (timestamp_nano() < measure_stop || !freq_filled) {
			std::uint32_t id1;
			std::uint32_t id2;
			std::uint64_t nano1;
			std::uint64_t tsc1;
			const std::uint64_t nano0 = timestamp_nano();
			do {
				nano1 = timestamp_nano();
				tsc1 = timestamp_tsc(id1);
			} while (nano0 == nano1);
			const std::uint64_t nano2 = nano1 + 100'000;
			std::uint64_t nano3;
			std::uint64_t tsc2;
			do {
				nano3 = timestamp_nano();
				tsc2 = timestamp_tsc(id2);
			} while (nano3 <= nano2);

			if (id1 == id2 && nano3 - nano2 < 1'000'000 && tsc2 > tsc1) {
				freq[freq_ind] = double(tsc2 - tsc1) / double(nano3 - nano1);
				freq_ind++;
				if (freq_ind == 100) {
					freq_filled = true;
					freq_ind = 0;
				}
			}
			else if (nano3 > measure_stop + 2'000'000'000ULL) {
				throw std::runtime_error("Determining TSC frequency failed!");
			}
		}
		std::qsort(
			freq,
			100,
			sizeof(double),
			[](const void* a, const void* b)->int {
				const auto first{ *static_cast<double const*>(a) };
				const auto second{ *static_cast<double const*>(b) };
				if (first < second) return -1;
				else if (first > second) return 1;
				else return 0; }
		);
		return freq[50];
	}
	
	double rdtsc_freq_GHz() {
		static double cpu_freq{ measure_freq() };
		return cpu_freq;
	}
}
