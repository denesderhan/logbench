//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#pragma once
#include <clipp.h>

#include <cstddef>
#include <string>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <charconv>

#include <logbench/small_string.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/store_path.hpp>
#include <version.hpp>

namespace {	
	template<typename T, T r0, T r1>
	bool check_range(std::string const& s) noexcept {
		T num{ 0 };
		auto result = std::from_chars(s.data(), s.data() + s.size(), num);
		if (result.ec == std::errc{}) {
			if (r0 <= r1) {
				return num >= r0 && num <= r1;
			}
			else {
				return num >= r1 && num <= r0;
			}
		}
		return false;
	}

	bool check_sink(std::string const& s) noexcept {
		if (s == "lib" || s == "null" || s == "sort" || s == "unsort") {
			return true;
		}
		return false;
	}

	bool check_formatter(std::string const& s) noexcept {
		if (s == "bin" || s == "txt") {
			return true;
		}
		return false;
	}
}

static std::string input_par_str(logbench::test_in_param& inpar) {
	std::string out;
	out += " -t ";
	out += std::to_string(inpar.thread_num);
	out += " -l ";
	out += std::to_string(inpar.log_call_per_thread);
	out += " -w ";
	out += std::to_string(inpar.work_cycles);
	out += " -d ";
	out += std::to_string(inpar.test_duration_target);
	out += " -f ";
	out += logbench::formatter_typ_txt(inpar.formatter_type);
	out += " -s ";
	out += logbench::sink_typ_txt(inpar.sink_type);
	out += " -p ";
	out += std::to_string(inpar.pin_to_cores.first);
	out += ' ';
	out += std::to_string(inpar.pin_to_cores.second);
	out += " --buffer_size ";
	out += std::to_string(inpar.lib_buffer_size);
	out += " --buffer_flush ";
	out += std::to_string(inpar.buffer_flush_time);
	out += " --sink_flush ";
	out += std::to_string(inpar.sink_flush_time);
	if (inpar.save_latency) out += " --latency ";
	if (inpar.log_self) out += " --logself ";
	if (!inpar.set_prio_high) out += " --noprio ";
	if (inpar.count_dropped) out += " --dropped ";
	return out;
}

inline bool parse_par(
	int argc, char* argv[], 
	logbench::test_in_param& par, 
	std::string& run_path,
	unsigned int& io_cooldown) 
{	
	std::string out_dir_cli;
	std::string bench_dir_cli;
	std::string formatter{ "txt" };
	std::string sink{ "lib" };
	par.save_latency = false;
	par.log_self = false;
	bool noprio{ false };
	bool fail{ false };
	bool end{ false };
	enum class mode { run, help, ver };
	auto cli = (
		clipp::option("--version").set(end).doc("Prints the version.") |
		clipp::option("--help").set(fail).doc("Prints help.") |
		(clipp::opt_value("benchmark", run_path).doc("Path to benchmark executable or directory."),
		clipp::option("-o", "--outdir").doc("Path to benchmark output (result) directory.") & clipp::value(clipp::match::nonempty, "path", out_dir_cli),
		clipp::option("-b", "--benchdir").doc("Path to log output (log) directory.") & clipp::value(clipp::match::nonempty, "path", bench_dir_cli),
		clipp::option("-t", "--threads").doc("Number of parallel logging threads.") & clipp::value(check_range<std::uintmax_t, 0, 1024>, "unsigned int", par.thread_num).doc("[0 - 1024]"),
		clipp::option("-l", "--lognum").doc("Number of log calls per thread.") & clipp::value(check_range < std::uintmax_t, 0, 1'000'000>, "unsigned int", par.log_call_per_thread).doc("[0 - 1'000'000]"),
		clipp::option("-d", "--duration").doc("Targeted duration of the individual benchmark runs in milliseconds.") & clipp::value(check_range < std::uintmax_t, 0, 3'600'000>, "millisec", par.test_duration_target).doc("[0 - 3'600'000]"),
		clipp::option("-w", "--work").doc("Amount of work to do, preceding every log call.") & clipp::value(check_range < std::uintmax_t, 0, 1'000'000'000>, "unsigned long", par.work_cycles).doc("[0 - 1'000'000'000]"),
		clipp::option("--buffersize").doc("Set memory used by the logging library in Kbytes per logging thread.") & clipp::value(check_range < std::uintmax_t, 1, 1'000'000>, "Kbyte", par.lib_buffer_size).doc("[1 - 1'000'000]"),
		clipp::option("--bufferflush").doc("Set logging library internal buffer flush interval in millieconds (0 means infinity).") & clipp::value(check_range < std::uintmax_t, 0, 3'600'000>, "millisec", par.buffer_flush_time).doc("[0 - 3'600'000]"),
		clipp::option("--sinkflush").doc("Set logging library sink flush interval in millieconds (0 means infinity)") & clipp::value(check_range < std::uintmax_t, 0, 3'600'000>, "millisec", par.sink_flush_time).doc("[0 - 3'600'000]"),
		clipp::option("-p", "--pin").doc("Pin logging threads to core range. ({-1,-1} means no pinning)")
			& clipp::value(check_range < std::intmax_t, -1, 1024>, "first core", par.pin_to_cores.first).doc("[0 - 1024] | -1") & clipp::value(check_range < std::intmax_t, -1, 1024>, "last core", par.pin_to_cores.second).doc("[0 - 1024] | -1"),
		clipp::option("-f", "--formatter").doc("Type of formatter to use.") & clipp::value(check_formatter, "string", formatter).doc("[txt | bin]"),
		clipp::option("-s", "--sink").doc("Type of sink to use.") & clipp::value(check_sink, "string", sink).doc("[null | sort | unsort]"),
		clipp::option("--latency").set(par.save_latency).doc("Output latency data. (creates .csv file in result/[test_id] directory)."),
		clipp::option("--logself").set(par.log_self).doc("Logging library self logging."),
		clipp::option("--noprio").set(noprio).doc("Do not set logging threads priority to high."),
		clipp::option("--dropped").set(par.count_dropped).doc("Count messages dropped by logging library."),
		clipp::option("-c", "--cooldown").doc("Pause between benchmark runs in millisec (used only if benchmark path is a directory).") & clipp::value(check_range < std::uintmax_t, 0, 3'600'000>, "millisec", io_cooldown).doc("[0 - 3'600'000]"))
	);
	
	if (!clipp::parse(argc, argv, cli)) {
		fail = true;
	}

	if (end) {
		std::cout << argv[0] << " version " << LOGBENCH_VERSION << '\n'
		<< "logbench.lib version " << LOGBENCHLIB_VERSION << '\n';
		return false;
	}

	par.set_prio_high = !noprio;

	std::filesystem::path out_dir; 
	if (!fail) {
		if (out_dir_cli.empty()) {
			std::size_t num{ 0 };
			do {
				out_dir = std::filesystem::absolute(std::filesystem::current_path() 
					/ (std::string("result_") + std::to_string(num)));
				num++;
			} while (std::filesystem::exists(out_dir) && num < 100);
			if (std::filesystem::exists(out_dir)) {
				std::cout << "Can not determine output path! Set it with --out_dir option!\n\n";
				fail = true;
			}
		}
		else {
			out_dir = std::filesystem::absolute(std::filesystem::path{ out_dir_cli });
		}
	}
		
	std::filesystem::path bench_dir; 
	
	if (!fail) {
		if (!bench_dir_cli.empty()) {
			bench_dir = std::filesystem::absolute(std::filesystem::path{ bench_dir_cli });
		}
		else {
			bench_dir = std::filesystem::absolute(std::filesystem::current_path());
		}
		auto log_path = bench_dir / "log";
		auto temp_path = bench_dir / "temp";

		store_path(par.out_path, out_dir);
		store_path(par.log_path, log_path);
		store_path(par.temp_path, temp_path);
		par.formatter_type = logbench::get_formatter_enum(formatter);
		par.sink_type = logbench::get_sink_enum(sink);
		if (par.pin_to_cores.first == -1 || par.pin_to_cores.second == -1) {
			par.pin_to_cores.first = -1;
			par.pin_to_cores.second = -1;
		}
	}

	if (fail) {
		std::cout << clipp::make_man_page(cli, argv[0]);
	}
	return !fail;
}
