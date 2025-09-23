#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import sys
from call_logbench import logbench

# Stress test usage conditions: no work symulated, logging frequency: infinite (system max), 4 threads

logbench(str(sys.argv[1]), "-o results -c 10000 -b ramdrive -t 4 -p 1 5 -l 100000 -w 0 -d 0 --dropped")
