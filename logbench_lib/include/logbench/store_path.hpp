//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <logbench/small_string.hpp>

#include <cstddef>
#include <filesystem>

namespace logbench {
	template <std::size_t N>
	void store_path(logbench::small_string<N>& sstr, std::filesystem::path const& path) {
		auto path_data = path.c_str();
		std::string out;
		while (*path_data != 0) {
			if (*path_data < 32 || *path_data > 126) {
				throw std::runtime_error("Path must be ASCII!");
			}
			out += static_cast<char>(*path_data++);
		}
		if (out.size() > sstr.capacity()) {
			throw std::runtime_error("Path too long!");
		}
		sstr = std::string_view{ out };
	}
}
