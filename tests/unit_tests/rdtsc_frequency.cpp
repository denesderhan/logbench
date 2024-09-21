//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <catch2/catch_all.hpp>

#include <iostream>
#include <cstdint>
#include <sstream>
#include <logbench/rdtsc_frequency.hpp>

TEST_CASE("rdtsc_frequency") {
	SECTION("uint32_t_1") {
		double freq = logbench::rdtsc_freq_GHz();
		CHECK(!std::isnan(freq));
		CHECK(freq > 0.0);
	};
}
