//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstddef>
#include <string>
#include <type_traits>

namespace logbench {
    enum class hex_format {

    };
    template <bool hex_prefix = true, typename T>
    std::string to_hexstring(T num) {
        static_assert(std::is_unsigned_v<T>);
        static constexpr const char* digits = "0123456789abcdef";
        constexpr std::size_t hexlen = (sizeof(T) << 1) + hex_prefix * 2;
        char string[hexlen];
		std::size_t ind = hexlen - 1;
        do {
            string[ind--] = digits[num & 0x0f];
            num >>= 4;
        } while (num > 0);
        if constexpr (hex_prefix) {
            string[ind--] = 'x';
            string[ind--] = '0';
        }
        ind++;
        return std::string(std::string_view{&string[ind], hexlen - ind});
    }
}
