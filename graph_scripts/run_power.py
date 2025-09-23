#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import sys
from call_logbench import logbench

# usage: run_power.py directoy

folder = str(sys.argv[1])

# power consumtion tests, 7 minutes duration

# Very high usage 500 Hz
logbench(folder, "-o results -c 5000 -b ramdrive -t 4 -p 1 5 -l 50000 -w 0 -d 100000 --dropped")

# high usage 50 Hz
logbench(folder, "-o results -c 5000 -b ramdrive -t 4 -p 1 5 -l 5000 -w 0 -d 100000 --dropped")

# low usage 0,5 Hz
logbench(folder, "-o results -c 5000 -b ramdrive -t 4 -p 1 5 -l 50 -w 0 -d 100000 --dropped")

# idle testing
logbench(folder, "-o results -c 5000 -b ramdrive -t 4 -p 1 5 -l 0 -w 0 -d 100000 --dropped")
