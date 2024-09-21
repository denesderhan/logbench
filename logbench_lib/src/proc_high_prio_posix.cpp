//Copyright © 2024, Dénes Derhán.
//Distributed under the MIT license (https://opensource.org/license/mit).
#ifndef _WIN32
#include <sys/time.h>
#include <sys/resource.h>

namespace logbench {
	void proc_high_prio() {
		setpriority(PRIO_PROCESS, 0, -10);
	}
}
#endif
