//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <string_view>
#include <stdexcept>

namespace logbench {
	enum class sink_typ : unsigned char {
		lib,
		null,
		unsort,
		sort
	};

	inline std::string_view sink_typ_txt(sink_typ f) noexcept {
		if (f == sink_typ::sort)  return "sort";
		if (f == sink_typ::unsort) return "unsort";
		if (f == sink_typ::null)  return "null";
		if (f == sink_typ::lib)  return "lib";
		return "unknown";
	}

	inline logbench::sink_typ get_sink_enum(std::string_view str) {
		if (str == "lib") return logbench::sink_typ::lib;
		if (str == "null") return logbench::sink_typ::null;
		if (str == "unsort") return logbench::sink_typ::unsort;
		if (str == "sort")	return logbench::sink_typ::sort;
		throw std::runtime_error("Unknown sink type!");
	}

}
