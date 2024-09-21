//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>

#include <logbench/force_inline.hpp>

namespace logbench {
	LOGBENCH_FORCEINLINE std::uint64_t do_work(std::uint64_t cycles, std::uint64_t in) noexcept {
		std::uint64_t out = in;
		for (std::uint64_t i = 0; i < cycles; ++i) {
			const std::uint64_t bit_noise1 = std::uint64_t{ 0xB5297A4DB5297A4DULL };
			const std::uint64_t bit_noise2 = std::uint64_t{ 0x68E31DA468E31DA4ULL };
			const std::uint64_t bit_noise3 = std::uint64_t{ 0x1B56C4E91B56C4E9ULL };

			out *= bit_noise1;
			out ^= (out >> 8);
			out += bit_noise2;
			out ^= (out << 8);
			out *= bit_noise3;
			out ^= (out >> 8);
		}
		return out;
	}
}
// Based on the GDC 2017 talk "Math for Game Programmers: Noise-Based RNG"
// https://www.youtube.com/watch?v=LWFzPP8ZbdU
