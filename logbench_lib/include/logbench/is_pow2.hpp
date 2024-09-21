//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <type_traits>

namespace logbench {
	template<typename T>
	inline constexpr bool is_pow2(T num) noexcept {
		static_assert(std::is_unsigned_v<T>, "Only for unsigned!");
		return num && ((num & (num - 1)) == 0);
	}
}
