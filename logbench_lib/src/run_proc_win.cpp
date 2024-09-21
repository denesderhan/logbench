//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#if defined(_WIN32)
#include <run_proc.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <string>
#include <string_view>

#include <logbench/timestamp_nano.hpp>
namespace logbench {
    void run_proc(std::string const& path, proc_run_stats& stats) noexcept {
        try {
            //not used, nothing to send/receive to/from child process
            HANDLE g_hChildStd_IN_Wr = NULL;
            HANDLE g_hChildStd_OUT_Rd = NULL;

            SECURITY_ATTRIBUTES saAttr;
            saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
            saAttr.lpSecurityDescriptor = NULL;
            saAttr.bInheritHandle = TRUE;

            STARTUPINFOA si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            si.dwFlags |= STARTF_USESTDHANDLES;

            // Create a pipe for the child process's STDOUT. 
            if (!CreatePipe(&g_hChildStd_OUT_Rd, &si.hStdOutput, &saAttr, 0)) {
                throw std::runtime_error("Process i/o pipe creation error!");
            }
            si.hStdError = si.hStdOutput;

            // Ensure the read handle to the pipe for STDOUT is not inherited.
            if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
                throw std::runtime_error("Process i/o pipe creation error!");
            }
            // Create a pipe for the child process's STDIN. 
            if (!CreatePipe(&si.hStdInput, &g_hChildStd_IN_Wr, &saAttr, 0)) {
                throw std::runtime_error("Process i/o pipe creation error!");
            }
            if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
                throw std::runtime_error("Process i/o pipe creation error!");
            }

            PROCESS_INFORMATION pi;
            ZeroMemory(&pi, sizeof(pi));

            stats.process_start_time = logbench::timestamp_nano<true>();
            if (!CreateProcessA(NULL,            // No module name (use command line)
                const_cast<char*>(path.c_str()), // command line
                NULL,                            // Process handle not inheritable
                NULL,                            // Thread handle not inheritable
                FALSE,                           // Set handle inheritance to FALSE
                0,                               // No creation flags
                NULL,                            // Use parent's environment block
                NULL,                            // Use parent's starting directory 
                &si,                             // Pointer to STARTUPINFOA structure
                &pi)                             // Pointer to PROCESS_INFORMATION structure
                )
            {
                throw std::runtime_error("Process creation error!");
            }

            // Wait until child process exits.
            WaitForSingleObject(pi.hProcess, INFINITE);
            stats.process_stop_time = logbench::timestamp_nano<true>();

            DWORD exit_code;
            if (!GetExitCodeProcess(
                pi.hProcess,
                &exit_code))
            {
                throw std::runtime_error("Query exit code error!");
            }
            if (exit_code != 0) {
                stats.exit_code = exit_code;
                throw std::runtime_error("Process exited with error!");
            }

            PROCESS_MEMORY_COUNTERS pmc;
            if (!GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof(pmc)))
            {
                throw std::runtime_error("Query memory stat error!");
            }
            else {
                stats.peak_memory = pmc.PeakWorkingSetSize;
                stats.page_faults = pmc.PageFaultCount;
            }

            ULONG64 cpu_cycles{ 0 };
            if (!QueryProcessCycleTime(pi.hProcess, &cpu_cycles))
            {
                throw std::runtime_error("Query cpu cycles error!");
            }
            else {
                stats.cpu_cycles = cpu_cycles;
            }

            /*
            DWORD dwRead;
            char buff{0};
            bool bSuccess = ReadFile(g_hChildStd_OUT_Rd, &buff, 1, &dwRead, NULL);
            if (bSuccess && dwRead != 0) {
                //process had an output (logging lib writes to stdout or stderr (messing up))
            }
            */

            // Close handles to the child process and its primary thread. 
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            // Close handles to the stdin and stdout pipes no longer needed by the child process.
            CloseHandle(si.hStdOutput);
            CloseHandle(si.hStdInput);
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
