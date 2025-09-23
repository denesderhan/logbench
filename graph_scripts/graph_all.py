#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import os
import graph_throughput
import graph_latency_stat
import graph_power
import graph_mem

os.system('python graph_latency_hist.py a')
os.system('python graph_latency_hist.py b')
os.system('python graph_latency_time.py')
