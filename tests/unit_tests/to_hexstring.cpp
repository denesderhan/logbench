//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <catch2/catch_all.hpp>

#include <iostream>
#include <cstdint>
#include <sstream>
#include <logbench/to_hexstring.hpp>

TEST_CASE("to_hexstring") {
	SECTION("uint32_t_1") {
		std::uint32_t num = 0;
		CAPTURE(num);
		auto result{logbench::to_hexstring<false>(num)};
		CHECK(result == "0");
		result = logbench::to_hexstring(num);
		CHECK(result == "0x0");

		num = UINT32_MAX;
		CAPTURE(num);
		result = logbench::to_hexstring<false>(num);
		CHECK(result == "ffffffff");
		result = logbench::to_hexstring(num);
		CHECK(result == "0xffffffff");

	};

	SECTION("uint32_t_rand") {
		std::uint32_t num = GENERATE(take(10000, random(std::uint32_t(0), std::uint32_t(UINT32_MAX))));
		CAPTURE(num);
		auto result{logbench::to_hexstring<false>(num)};
		std::stringstream exp;
		exp << std::hex << num;
		CHECK(result == exp.str());
		result = logbench::to_hexstring(num);
		std::string exp2{"0x"};
		exp2.append(exp.str());
		CHECK(result == exp2);

	};

	SECTION("uint64_t_1") {
		std::uint64_t num = 0;
		CAPTURE(num);
		auto result{ logbench::to_hexstring<false>(num) };
		CHECK(result == "0");
		result = logbench::to_hexstring(num);
		CHECK(result == "0x0");

		num = UINT64_MAX;
		CAPTURE(num);
		auto result2 = logbench::to_hexstring<false>(num);
		CHECK(result2 == "ffffffffffffffff");
		result = logbench::to_hexstring(num);
		CHECK(result == "0xffffffffffffffff");
	};

	SECTION("uint64_t_rand") {
		std::uint64_t num = GENERATE(take(10000, random(std::uint32_t(0), std::uint32_t(UINT64_MAX))));
		CAPTURE(num);
		auto result{logbench::to_hexstring<false>(num)};
		std::stringstream exp;
		exp << std::hex << num;
		CHECK(result == exp.str());
		result = logbench::to_hexstring(num);
		std::string exp2{"0x"};
		exp2.append(exp.str());
		CHECK(result == exp2);
	};
}
