//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <logbench/test_run.hpp>

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>
#include <cmath>
#pragma intrinsic(memcpy)

#include <logbench/is_exec.hpp>
#include <logbench/rdtsc_frequency.hpp>
#include <logbench/store_path.hpp>
#include <logbench/test_result_thread.hpp>
#include <run_proc.hpp>

namespace logbench {

	test_run::test_run() = default;

	test_run::~test_run() noexcept {
		b_data_.remove();
	}

	void test_run::set_parameters(test_in_param const& input_param) {
		b_data_.create(input_param);
		b_data_.clear_result();
		in_param_ = input_param;
	}
	
	intmax_t test_run::run(std::string const& exec_path) noexcept
	{
		try {
			if (!is_exec(std::filesystem::path{ exec_path })) {
				throw std::runtime_error("Path is not executable!");
			}

			const auto temp_p{ std::string_view{ in_param_.temp_path }  };
			if (std::filesystem::exists(temp_p)) {
				for (const auto& entry : std::filesystem::directory_iterator(temp_p))
					std::filesystem::remove_all(entry.path());
			}
			else std::filesystem::create_directory(temp_p);

			const auto log_p{ std::string_view{ in_param_.log_path }  };
			if (std::filesystem::exists(log_p)) {
				for (const auto& entry : std::filesystem::directory_iterator(log_p))
					std::filesystem::remove_all(entry.path());
			}
			else std::filesystem::create_directory(log_p);

			b_data_.clear_result();
			out_param_ = test_out_param{};
			out_param_.rdtsc_freq = logbench::rdtsc_freq_GHz();
			b_data_.set_data(out_param_);
			run_proc(exec_path, run_stats_);
			b_data_.get_data(out_param_);
			if (run_stats_.exit_code == 0) {
				if (out_param_.test_name.empty()) {
					throw std::runtime_error("Binary was not a test (test name empty)!");
				}
				else if (out_param_.lib_name.empty()) {
					throw std::runtime_error("Test did not set logging library name!");
				}
			}
		}
		catch(const std::exception& ex){
			run_stats_.exit_code = 1;
			run_stats_.error = small_string<128>{ ex.what() };
		}
		return run_stats_.exit_code;
	}

	void test_run::get_data(test_in_param& data) const noexcept
	{
		data = in_param_;
	}

	void test_run::get_data(test_out_param& data) const noexcept
	{
		data = out_param_;
	}

	void test_run::get_data(proc_run_stats& data) const noexcept
	{
		data = run_stats_;
	}

	std::string test_run::result() const
	{
		std::string test_line;

		test_line += to_hexstring(run_stats_.process_start_time) + ',';
		test_line.append(out_param_.lib_name );
		test_line += ',';
		test_line.append(out_param_.lib_version );
		test_line += ',';
		test_line += std::to_string(
			double(run_stats_.process_stop_time - run_stats_.process_start_time) / 1'000'000'000.0) + ',';
		test_line += std::to_string(out_param_.rdtsc_freq) + ',';
		test_line += std::to_string(b_data_.avg_latency_tsc() / out_param_.rdtsc_freq) + ',';
		test_line += std::to_string(throughput()) + ',';
		test_line += std::to_string(run_stats_.cpu_cycles) + ',';
		test_line += std::to_string(run_stats_.peak_memory) + ',';
		test_line += std::to_string(run_stats_.page_faults) + ',';
		test_line += std::to_string(out_param_.buffer_flush_time) + ',';
		test_line += std::to_string(out_param_.sink_flush_time) + ',';
		test_line += std::to_string(out_param_.lib_buffer_size) + ',';
		test_line.append(logbench::formatter_typ_txt(out_param_.formatter_type));
		test_line += ',';
		test_line += out_param_.formater_impl;
		test_line += ',';
		test_line.append(logbench::sink_typ_txt(out_param_.sink_type));
		test_line += ',';
		if (out_param_.debug_build) {
			test_line += "debug,";
		}
		else {
			test_line += "release,";
		}
		const auto drop{count_dropped()};
		if (drop < 0) {
			test_line += '?';
		}
		else {
			test_line += std::to_string(drop);
		}


		if (in_param_.save_latency && in_param_.log_call_per_thread != 0 && in_param_.thread_num != 0) {
			std::vector<double> latencies{b_data_.get_latencies()};
			if (!latencies.empty()) {
				std::sort(latencies.begin(), latencies.end());
				test_line += ',' + std::to_string(latencies.at(0));
				test_line += ',' + std::to_string(*(latencies.end() - 1));
				test_line += ',' + std::to_string(median(latencies));
				double val75 = nth_val(latencies, 75.0);
				test_line += ',' + std::to_string(val75 - nth_val(latencies, 25.0));
				test_line += ',' + std::to_string(val75);
				test_line += ',' + std::to_string(nth_val(latencies, 90.0));
				test_line += ',' + std::to_string(nth_val(latencies, 99.0));
				test_line += ',' + std::to_string(nth_val(latencies, 99.9));
			}
		}
		return test_line;
	}
	
	void test_run::save_result() const {
		std::filesystem::path save_path{ std::string_view{ in_param_.out_path } };
		if (!std::filesystem::exists(save_path)) std::filesystem::create_directory(save_path);
		save_path /= test_id().append(".csv");
		const bool write_header{ !std::filesystem::exists(save_path) };
				
		std::ofstream out_file;
		out_file.open(
			save_path,
			std::ios_base::app
		);
		if (!out_file.good()) {
			throw std::runtime_error("Test result file path invalid!");
		}
		if (write_header) {
			out_file << result_header_;
			out_file << "\n";
		}
		out_file << result() << '\n';
	}
	
	void test_run::save_latencies() const {
		if (!in_param_.save_latency) return;
		std::filesystem::path latencyfile_path{ std::string_view{ in_param_.out_path }   };
		if (!std::filesystem::exists(latencyfile_path)) std::filesystem::create_directory(latencyfile_path);
		latencyfile_path /= test_id();
		if (!std::filesystem::exists(latencyfile_path)) std::filesystem::create_directory(latencyfile_path);
		latencyfile_path /= latency_file_name();

		std::ofstream latency_file;
		latency_file.open(
			latencyfile_path,
			std::ios_base::trunc
		);
		if (!latency_file.good()) {
			throw std::runtime_error("Latency file path invalid!");
		}
		std::uint64_t min_start{ UINT64_MAX };
		for (unsigned int t = 0; t < in_param_.thread_num; t++) {
			std::uint64_t first_time;
			memcpy(
				&first_time,
				b_data_.latency_data(t),
				sizeof(std::uint64_t)
			);
			if (first_time < min_start) {
				min_start = first_time;
			}
		}

		latency_file << latency_result_header_ << "\n";
		for (unsigned int t = 0; t < in_param_.thread_num; t++) {
			logbench::test_result_thread thr_data;
			b_data_.get_data(t, thr_data);
			auto dat_ptr = b_data_.latency_data(t);
			for (int l = 0; l < thr_data.log_num; l++) {
				std::uint64_t start;
				memcpy(
					&start,
					dat_ptr,
					sizeof(std::uint64_t)
				);
				dat_ptr += sizeof(std::uint64_t);
				std::uint64_t stop;
				memcpy(
					&stop,
					dat_ptr,
					sizeof(std::uint64_t)
				);
				dat_ptr += sizeof(std::uint64_t);
				std::uint32_t core1;
				memcpy(
					&core1,
					dat_ptr,
					sizeof(std::uint32_t)
				);
				dat_ptr += sizeof(std::uint32_t);
				std::uint32_t core2;
				memcpy(
					&core2,
					dat_ptr,
					sizeof(std::uint32_t)
				);
				dat_ptr += sizeof(std::uint32_t);

				latency_file << double(start - min_start) / out_param_.rdtsc_freq;
				latency_file << ',' << double(stop - start) / out_param_.rdtsc_freq;
				latency_file << ',' << t;
				latency_file << ',' << core1;
				latency_file << ',' << core2;
				latency_file << "\n";
			}
		}
	}

	std::string test_run::latency_file_name() const {
		std::string out;
		out.append(out_param_.lib_name );
		out += +'_';
		out.append(out_param_.lib_version );
		out += +'_';
		out.append(logbench::formatter_typ_txt(out_param_.formatter_type));
		out += +'_';
		out.append(logbench::sink_typ_txt(out_param_.sink_type));
		out += +'_';
		if (out_param_.debug_build) {
			out.append("debug");
		}
		else {
			out.append("release");
		}
		//out += '_';
		//out += to_hexstring(run_stats_.process_start_time);
		out.append(".csv");
		return out;
	}

	std::string test_run::test_id() const {
		std::string id{ out_param_.test_name };
		id += "_t" + std::to_string(in_param_.thread_num);
		id += "_l" + std::to_string(in_param_.log_call_per_thread);
		id += "_w" + std::to_string(in_param_.work_cycles);
		id += "_d" + std::to_string(in_param_.test_duration_target);
		if (in_param_.save_latency) {
			id += "_latency";
		}

		std::string hashed_part{ hashed_id_part() };
		auto hash = fnv_1a_32(hashed_part.data(), hashed_part.size());
		id += '_';
		id += to_hexstring(hash);
		return id;
	}

	std::string test_run::hashed_id_part() const {
		std::string hashed_part;
		hashed_part += "buffer";
		hashed_part += std::to_string(in_param_.lib_buffer_size);
		hashed_part += "_poll";
		hashed_part += std::to_string(in_param_.buffer_flush_time);
		hashed_part += "_sinkflush";
		hashed_part += std::to_string(in_param_.sink_flush_time);
		hashed_part += "_pin";
		hashed_part += std::to_string(in_param_.pin_to_cores.first);
		hashed_part += 'p';
		hashed_part += std::to_string(in_param_.pin_to_cores.second);
		hashed_part += "_self";
		hashed_part += std::to_string(in_param_.log_self);
		hashed_part += "_prio";
		hashed_part += std::to_string(in_param_.set_prio_high);
		return hashed_part;
	}
	
	double test_run::throughput() const noexcept {
		if (run_stats_.process_stop_time <= out_param_.logging_begin_time) return 0.0;
		double seconds = double(run_stats_.process_stop_time - out_param_.logging_begin_time) / 1'000'000'000.0;
		return double(std::uint64_t(in_param_.log_call_per_thread) * in_param_.thread_num) / seconds;
	}

	std::intmax_t test_run::count_dropped() const {
		if (in_param_.count_dropped &&
			out_param_.formatter_type == logbench::formatter_typ::txt)
		{
			const auto num_expected = std::intmax_t(in_param_.log_call_per_thread) * in_param_.thread_num;
			std::intmax_t log_num = count_lines();
			if (num_expected == 0 && log_num == 0) return 0;
			if (log_num >= out_param_.garbage_lines) {
				log_num -= out_param_.garbage_lines;
				if (log_num <= num_expected) {
					return num_expected - log_num;
				}
			}
		}
		return -1;
	}

	std::intmax_t test_run::count_lines() const {
		std::filesystem::path log_dir(std::string_view{ in_param_.log_path });
		if (!std::filesystem::exists(log_dir)) return 0;
		std::filesystem::directory_iterator it(log_dir);
		if (it == std::filesystem::directory_iterator{}
			|| !std::filesystem::is_regular_file(*it)) return 0;
		std::ifstream log_file(std::filesystem::path{ *it }, std::ios::binary);
		char b;
		std::intmax_t line_count{ 0 };
		while (log_file.read(&b, 1 ).good()) {
			if (b == '\n') ++line_count;
		}
		return line_count;
	}

	double test_run::median(std::vector<double> const& input) const {
		std::size_t n = input.size();
		if (n == 1) {
			return input.at(0);
		}
		else if (n % 2 == 0) {
			std::size_t i = n / 2;
			return (input.at(i - 1) + input.at(i)) / 2.0;
		}
		else {
			return input.at(n / 2);
		}
	}

	double test_run::nth_val(std::vector<double> const& sorted_input, double n) const {
		assert(!sorted_input.empty());
		n = std::clamp(n, 1.0, 100.0);
		double index_dbl = std::trunc(double(sorted_input.size()) * (n / 100.0));
		std::size_t ind = std::size_t(index_dbl);
		ind = std::clamp(ind, std::size_t(0), sorted_input.size() - 1);
		return sorted_input.at(ind);
	}
}
