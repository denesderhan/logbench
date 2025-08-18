//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#ifndef _WIN32
#include <stdexcept>
#include <sys/resource.h>
#include <unistd.h>

#include <logbench/thread_high_prio.hpp>

namespace logbench {
    void thread_high_prio() {
        /*
        By default , threads in Linux are scheduled using the SCHED_OTHER policy, which is a time-sharing policy.
        Threads with SCHED_OTHER are scheduled based on their dynamic priority, 
        which is influenced by factors like CPU usage and niceness value.
        The niceness value is affecting the priority of the whole process,
        and thus the threads within it. Lower niceness values mean higher priority.
        We use the niceness value instead of SCHED_FIFO or SCHED_RR policies, 
        to prevent potential starvation of other threads.
        */
        
        const int new_nice = -10; // Higher priority than default
        if (setpriority(PRIO_PROCESS, getpid(), new_nice) != 0) {
            throw std::runtime_error("Could not set priority to high! (try running as root)");
        }
    }
}
#endif
