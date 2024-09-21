#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import subprocess
import sys

def run_tests(param_str) :
	to_run = "logbench "
	to_run += str(sys.argv[1]) + ' ' + param_str
	subprocess.run(to_run.split())

run_tests("-o results -c 10000 -b ramdrive -t 4 -p 1 5 -l 100000 -w 0 -d 0 --dropped")
run_tests("-o results -c 10000 -b ramdrive -t 4 -p 1 5 -l 1500 -w 100000 -d 30000 --latency --dropped")



