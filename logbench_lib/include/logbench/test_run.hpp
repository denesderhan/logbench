//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>
#include <logbench/proc_run_stats.hpp>
#include <logbench/bench_data.hpp>
#include <logbench/api_def.hpp>

namespace logbench {
	 class test_run {
	public:
		LOGBENCH_API test_run();
		LOGBENCH_API ~test_run() noexcept;
		LOGBENCH_API void set_parameters(test_in_param const& input_param);
		LOGBENCH_API std::intmax_t run(std::string const& exec_path) noexcept;
		LOGBENCH_API void get_data(test_in_param& data) const noexcept;
		LOGBENCH_API void get_data(test_out_param& data) const noexcept;
		LOGBENCH_API void get_data(proc_run_stats& data) const noexcept;
		LOGBENCH_API std::string result() const;
		LOGBENCH_API void save_result() const;
		LOGBENCH_API void save_latencies() const;
		constexpr std::string_view header() const noexcept {
			return result_header_;
		}
		LOGBENCH_API std::string latency_file_name() const;
		
		std::string error() const {
			std::string err{ run_stats_.error };
			err += ' ';
			err += out_param_.out_message;
			return err;
		}
		LOGBENCH_API std::string hashed_id_part() const;
		LOGBENCH_API std::string test_id() const;
		LOGBENCH_API double throughput() const noexcept;
		LOGBENCH_API std::intmax_t count_dropped() const;
	private:

		std::intmax_t count_lines() const;
		double median(std::vector<double> const& input) const;
		double nth_val(std::vector<double> const& sorted_input, double n) const;

		test_in_param in_param_;
		test_out_param out_param_;
		proc_run_stats run_stats_;
		bench_data b_data_;
		static constexpr std::string_view result_header_{"start_time,lib_name,lib_ver,test_duration,cpu_freq,latency_mean,throughput,cpu_cycles,peak_memory,page_faults,buff_flush,sink_flush,buff_size,formatter,formatter_impl,sink,build,dropped,latency_min,latency_max,latency_median,latency_IQR,latency_75th,latency_90th,latency_99th,latency_99.9th"};
		static constexpr std::string_view latency_result_header_{"time,latency,thread,start_core,end_core"};
	};
}
