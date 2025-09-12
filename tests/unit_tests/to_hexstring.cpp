//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <catch2/catch_all.hpp>

#include <logbench/to_hexstring.hpp>

#include <limits>
#include <string>
#include <string_view>
#include <iostream>
#include <cstdint>
#include <sstream>

TEST_CASE("to_hexstring") {
    SECTION("uint32_t") {
        auto data = GENERATE(
            std::pair<std::uint32_t, std::string_view>{0ull, "0x0"},
            std::pair<std::uint32_t, std::string_view>{(std::numeric_limits<std::uint32_t>::max)(), "0xffffffff"},
            std::pair<std::uint32_t, std::string_view>{1ull, "0x1"}, 
            std::pair<std::uint32_t, std::string_view>{0x1234abcdull, "0x1234abcd"}, 
            std::pair<std::uint32_t, std::string_view>{0xef56789ull, "0xef56789"}, 
            std::pair<std::uint32_t, std::string_view>{0xfffffffeull, "0xfffffffe"}, 
            std::pair<std::uint32_t, std::string_view>{0xfffffffull, "0xfffffff"});
        
        auto num = data.first;
        auto str = data.second;
        CAPTURE(num);
        auto result = logbench::to_hexstring(num);
        CHECK(result == str);
        result = logbench::to_hexstring<false>(num);
        CHECK(result == std::string_view{ str.data() + 2, str.size() - 2});        
    };

    SECTION("uint64_t") {
        auto data = GENERATE(
            std::pair<std::uint64_t, std::string_view>{0ull, "0x0"},
            std::pair<std::uint64_t, std::string_view>{(std::numeric_limits<std::uint64_t>::max)(), "0xffffffffffffffff"},
            std::pair<std::uint64_t, std::string_view>{1ull, "0x1"},
            std::pair<std::uint64_t, std::string_view>{0x1234abcdull, "0x1234abcd"},
            std::pair<std::uint64_t, std::string_view>{0xef56789ull, "0xef56789"},
            std::pair<std::uint64_t, std::string_view>{0xabcdef1234567890ull, "0xabcdef1234567890"},
            std::pair<std::uint64_t, std::string_view>{0xfffffffffffffffeull, "0xfffffffffffffffe"},
            std::pair<std::uint64_t, std::string_view>{0xfffffffffffffffull, "0xfffffffffffffff"});

        auto num = data.first;
        auto str = data.second;
        CAPTURE(num);
        auto result = logbench::to_hexstring(num);
        CHECK(result == str);
        result = logbench::to_hexstring<false>(num);
        CHECK(result == std::string_view{ str.data() + 2, str.size() - 2 });
    };
}
