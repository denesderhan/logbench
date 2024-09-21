//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <string_view>
#include <stdexcept>

namespace logbench {
	enum class formatter_typ : unsigned char {
		bin,
		txt
	};

	inline std::string_view formatter_typ_txt(formatter_typ f) noexcept {
		if (f == formatter_typ::bin) return "bin";
		if (f == formatter_typ::txt) return "txt";
		return "unknown";
	}

	inline logbench::formatter_typ get_formatter_enum(std::string_view str) {
		if (str == "bin") return logbench::formatter_typ::bin;
		if (str == "txt") return logbench::formatter_typ::txt;
		throw std::runtime_error("Unknown formatter type!");
	}

}
