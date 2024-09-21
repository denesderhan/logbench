//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <logbench/pin_thread.hpp>

#include <thread>

namespace logbench {
	void pin_thread::set_pins(std::pair<int, int> core_interval) noexcept {
		auto processor_count = std::thread::hardware_concurrency();
		if (processor_count == 0) processor_count = 1;

		//disable pinning
		if (core_interval.first < 0
			|| core_interval.second < 0
			|| core_interval.first > core_interval.second
			|| processor_count == 1
			|| static_cast<unsigned int>(core_interval.second) > processor_count - 1)
		{
			thread_to_core_.fill(-1);
			return;
		}

		auto core = core_interval.first;
		for (int i = 0; i < thread_to_core_.size(); i++) {
			thread_to_core_[i] = core++;
			if (core > core_interval.second) core = core_interval.first;
		}
	}
}
