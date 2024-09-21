//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#if defined(_WIN32)
#include <logbench/is_exec.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>
#include <logbench/store_path.hpp>

namespace logbench {
	bool is_exec(std::filesystem::path const& path) {
		if (!std::filesystem::is_regular_file(path)) {
			return false;
		}
		logbench::small_string<256> c_path;
		store_path(c_path, path);
		if (path.string().size() != c_path.size()) {
			return false;
		}
		DWORD exe_type;
		if (!GetBinaryTypeA(c_path.data(), &exe_type)) {
			return false;
		}
		return true;
	}
}

#endif
