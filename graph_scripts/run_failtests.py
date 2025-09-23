#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import sys
from call_logbench import logbench

# usage: run_failtests.py directory

folder = str(sys.argv[1])
logbench(folder, "-o fail_res1 -c 0 -b ramdrive -t 4 -p 1 5 -l 2000 -w 0 -d 0 --dropped -f txt --logself")
logbench(folder, "-o fail_res1 -c 0 -b ramdrive -t 4 -p 1 5 -l 2000 -w 0 -d 0 --dropped -f txt")
logbench(folder, "-o fail_res2 -c 0 -b ramdrive -t 4 -p 1 5 -l 2000 -w 0 -d 0 --dropped -f txt --logself -s null")
logbench(folder, "-o fail_res2 -c 0 -b ramdrive -t 4 -p 1 5 -l 2000 -w 0 -d 0 --dropped -f txt -s null")
