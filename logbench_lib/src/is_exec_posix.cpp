//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#ifndef _WIN32
#include <logbench/is_exec.hpp>

#include <unistd.h>
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
		if (access(c_path.data(), X_OK) == 0) {
			return true;
		}
		return false;
	}
}

#endif
