//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#ifndef _WIN32
#include <run_proc.hpp>

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstdint>
#include <string>
#include <string_view>

#include <ipc_data_posix.hpp>
#include <logbench/timestamp_nano.hpp>
#include <logbench/rdtsc_frequency.hpp>

namespace logbench {
    void run_proc(std::string const& path, proc_run_stats& stats) noexcept {
        try {
            stats.process_start_time = logbench::timestamp_nano<true>();
			pid_t pid{0};
			int status{0};
			rusage usage;

			if ((pid = fork()) == 0)
			{
				//redirect stdout,stderr to /dev/null
				int fd = open("/dev/null", O_WRONLY);
				dup2(fd, fileno(stdout));
				dup2(fd, fileno(stderr));
				{
					//destructor of ipc_data unmaps shared memory and closes file descriptor
					ipc_data ipc;
					//shm_unlink not called
				}
				execl(path.c_str(), path.c_str(), (char*)NULL);
			}
			else if (pid > 0)
			{
				wait4(pid, &status, 0, &usage);
			}
			else
			{
				throw std::runtime_error("Process creation error!");
			}
			
			if (status != 0) {
				stats.exit_code = status;
				throw std::runtime_error("Process exited with error!");
			}

			stats.process_stop_time = logbench::timestamp_nano<true>();		
			stats.peak_memory = usage.ru_maxrss * 1024;
			stats.page_faults = usage.ru_majflt;
            
			std::uint64_t cpu_time_usec = (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1'000'000ULL
				+ usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
			stats.cpu_cycles = cpu_time_usec * (std::uint64_t)(rdtsc_freq_GHz() * 1'000.0);
				                       
            stats.exit_code = 0;
            stats.error = "";
        }
        catch (const std::exception& ex) {
            if (stats.exit_code == 0) {
                stats.exit_code = 1;
            }
	    stats.error = small_string<128>{ ex.what() };
        }
    }
}
#endif
