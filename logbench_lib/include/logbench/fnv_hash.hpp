//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstddef>
#include <cstdint>
#include <climits>
#include <type_traits>

namespace logbench {
	template<typename T>
	inline constexpr std::uint32_t fnv_1a_32(const T* data, std::size_t size) noexcept {
		static_assert(CHAR_BIT == 8, "Error, CHAR_BIT != 8");
		static_assert(std::is_integral_v<T> && sizeof(T) == 1, "Only char and UTF8 supported");
		static_assert(std::is_unsigned_v<T> || (T(-1) & T(3)) == 3, "Not twos complement!");
		std::uint32_t hash = std::uint32_t{ 2166136261UL };
		auto char_ptr = data;
		const auto data_end = char_ptr + size;
		while (char_ptr < data_end) {
			hash ^= static_cast<std::uint32_t>(static_cast<unsigned char>(*char_ptr++));
			hash *= std::uint32_t{ 16777619UL };
		}
		return hash;
	}

	template<typename T>
	inline constexpr std::uint64_t fnv_1a_64(const T* data, std::size_t size) noexcept {
		static_assert(CHAR_BIT == 8, "Error, CHAR_BIT != 8");
		static_assert(std::is_integral_v<T> && sizeof(T) == 1, "Only char and UTF8 supported");
		static_assert(std::is_unsigned_v<T> || (T(-1) & T(3)) == 3, "Not twos complement!");
		std::uint64_t hash = std::uint64_t{ 14695981039346656037ULL };
		auto char_ptr = data;
		const auto data_end = char_ptr + size;
		while (char_ptr < data_end) {
			hash ^= static_cast<std::uint64_t>(static_cast<unsigned char>(*char_ptr++));
			hash *= std::uint64_t{ 0x100000001b3ULL };
		}
		return hash;
	}
}
