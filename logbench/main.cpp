//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#include <string>
#include <iostream>
#include <thread>
#include <filesystem>
#include <stdexcept>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <random>

#include <logbench/test_in_param.hpp>
#include <logbench/test_run.hpp>
#include <logbench/is_exec.hpp>
#include <logbench/proc_max_power.hpp>
#include <logbench/rdtsc_frequency.hpp>
#include <logbench/do_work.hpp>
#include <logbench/timestamp_nano.hpp>
#include <parse_par.hpp>

static void run_test(logbench::test_run& test, std::string const& run_path) noexcept {
	const auto errc = test.run(run_path);
	if (errc == 0) {
		try {
			test.save_latencies();
			test.save_result();
			std::cout << "OK," << '\n';
		}
		catch (const std::exception& ex) {
			std::cout << "ERROR," << ex.what() << '\n';
		}
	}
	else {
		std::cout << "ERROR," << test.error() << " Exit code: " << errc << '\n';
	}
}

int main(int argc, char* argv[])
{
	logbench::proc_max_power pow_g;
	try {
		logbench::test_in_param input_par;
		input_par.pin_to_cores.first = 0;
		std::size_t max_core{ std::size_t(std::thread::hardware_concurrency()) - 1};
		if (max_core > 1024) max_core = 1024;
		input_par.pin_to_cores.second = static_cast<int>(max_core);

		std::string run_path;
		//msec wait between test runs, default worst case (same hdd for log and result outputs)
		unsigned int pause{ 0 };
		if(!parse_par(argc, argv, input_par, run_path, pause)) return 1;

		logbench::test_run test;
		test.set_parameters(input_par);

		if (!std::filesystem::exists(std::string_view{ input_par.out_path }))
			std::filesystem::create_directories(std::string_view{ input_par.out_path });
		if (!std::filesystem::exists(std::string_view{ input_par.log_path }))
			std::filesystem::create_directories(std::string_view{ input_par.log_path });
		if (!std::filesystem::exists(std::string_view{ input_par.temp_path }))
			std::filesystem::create_directories(std::string_view{ input_par.temp_path });

		if (run_path.empty()) {
			std::cout << "Enter path of benchmark binary or directory of binaries:\n";
			std::cin >> run_path;
		}
		
		if (std::filesystem::is_directory(run_path)) {
			std::vector<std::filesystem::path> benchmarks;
			for (const auto& dir_entry : std::filesystem::recursive_directory_iterator(run_path)) {
				const auto path{ dir_entry.path() };
				if (logbench::is_exec(path)) {
					benchmarks.push_back(path);
				}
			}
						
			auto rand_beg{ benchmarks.begin() };
			auto noop_pos = std::find_if(benchmarks.begin(), benchmarks.end(), 
				[](const std::filesystem::path& p) { 
					return p.stem().string().find("noop") != std::string::npos; }
			);
			if (noop_pos != benchmarks.end()) {
				rand_beg++;
				if (noop_pos != benchmarks.begin()) {
					benchmarks.at(0).swap(*noop_pos);
				}
			}
			auto rd = std::random_device{};
			auto rng = std::default_random_engine{ rd() };
			std::shuffle(rand_beg, benchmarks.end(), rng);
			
			bool first = true;
			if (!benchmarks.empty()) {
				std::cout << "(TSC freq: " << logbench::rdtsc_freq_GHz() << " Ghz)\n";
			}
			std::uint64_t dummy{ 0 };
			for (const auto& path_entry : benchmarks) {
				const auto bench_path{ path_entry.string() };
				std::cout << bench_path << ',' << input_par_str(input_par) << ',' << std::flush;
				if (first) {
					//Dont have to wait.
					first = false;
				}
				else if (pause != 0) {
					//sleep
					std::this_thread::sleep_for(std::chrono::milliseconds(pause));
					//warmup
					const auto stop_time{logbench::timestamp_nano() + 2'000'000'000ULL};
					while (logbench::timestamp_nano() < stop_time) {
						dummy = logbench::do_work(1000, dummy);
					}
				}
				run_test(test, bench_path);
			}
			if (!benchmarks.empty() && pause != 0) {
				std::cout << std::hex << dummy << '\n';
			}
		}
		else {
			if (!logbench::is_exec(run_path)) {
				throw std::runtime_error("Path is not executable!");
			}
			std::cout << "(TSC freq: " << logbench::rdtsc_freq_GHz() << " Ghz)\n";
			std::cout << run_path << ',' << input_par_str(input_par) << ',' << std::flush;
			run_test(test, run_path);
		}
		std::cout << '\n';
		return 0;
	}
	catch (const std::exception& ex) {
		std::cout << "\n\nlogbench aborted with ERROR: " << ex.what() << "\n\n";
		return 1;
	}
}
