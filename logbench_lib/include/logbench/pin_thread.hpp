//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <utility>
#include <array>

#include <logbench/api_def.hpp>

namespace logbench {
	class pin_thread {
	public:
		LOGBENCH_API static void set_pins(std::pair<int, int> core_interval) noexcept;
		LOGBENCH_API static void pin(int thread_number);

	private:
		inline static std::array<int, 32> thread_to_core_{};
	};
}
