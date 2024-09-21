#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import subprocess
import sys

def run_tests(param_str) :
	to_run = "logbench "
	to_run += str(sys.argv[1]) + ' ' + param_str
	subprocess.run(to_run.split())

run_tests("-o fail_res -c 0 -b ramdrive -t 4 -p 1 5 -l 2000 -w 0 -d 0 --dropped -f txt --logself")
run_tests("-o fail_res -c 0 -b ramdrive -t 4 -p 1 5 -l 2000 -w 0 -d 0 --dropped -f txt")
run_tests("-o fail_res -c 0 -b ramdrive -t 4 -p 1 5 -l 2000 -w 0 -d 0 --dropped -f txt --logself -s null")
run_tests("-o fail_res -c 0 -b ramdrive -t 4 -p 1 5 -l 2000 -w 0 -d 0 --dropped -f txt -s null")

