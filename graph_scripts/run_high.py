#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import sys
from call_logbench import logbench

# High usage conditions: work symulated, logging frequency: 50 Hz/thread, 4 threads, 30 + 10 sec length

logbench(str(sys.argv[1]), "-o results -c 10000 -b ramdrive -t 4 -p 1 5 -l 1500 -w 100000 -d 30000 --latency --dropped")
