//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <logbench/api_def.hpp>
#include <filesystem>

namespace logbench {
	LOGBENCH_API bool is_exec(std::filesystem::path const& path);
}
