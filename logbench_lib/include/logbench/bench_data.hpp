//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstddef>
#include <vector>
#include <logbench/api_def.hpp>

namespace logbench {
	class ipc_data;
	struct test_in_param;
	struct test_out_param;
	struct test_result_thread;
	class bench_data {
	public:
		LOGBENCH_API bench_data() noexcept;
		LOGBENCH_API ~bench_data() noexcept;

		bench_data(bench_data const&) = delete;
		bench_data& operator=(bench_data const&) = delete;
		bench_data(bench_data&&) = delete;
		bench_data& operator=(bench_data&&) = delete;

		LOGBENCH_API void create(test_in_param const& data);
		LOGBENCH_API void open();
		LOGBENCH_API void remove() noexcept;
		LOGBENCH_API void get_data(test_in_param& data) const noexcept;
		LOGBENCH_API void set_data(test_out_param const& data) noexcept;
		LOGBENCH_API void get_data(test_out_param& data) const noexcept;
		LOGBENCH_API void set_data(unsigned int thread_index, test_result_thread const & data) noexcept;
		LOGBENCH_API void get_data(unsigned int thread_index, test_result_thread& data) const noexcept;
		LOGBENCH_API char* latency_data(unsigned int thread_index) const noexcept;
		LOGBENCH_API bool initialized() const;
		LOGBENCH_API void clear_result();
		LOGBENCH_API double avg_latency_tsc() const;
		LOGBENCH_API std::vector<double> get_latencies() const;
	private:
		void set_data_sizes() noexcept;
		char* thread_data(unsigned int thread_index) const noexcept;

		static ipc_data& ipc_buffer();

		inline static std::size_t page_size_{ 4096 };
		inline static std::size_t test_data_size_{ 0 };
		inline static std::size_t thread_data_size_{ 0 };

		inline static unsigned int thread_num_{ 0 };
		inline static unsigned int log_num_{ 0 };
		inline static bool latencies_{ false };
	};
}
