#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import os
import mem
import throughput
import latency_stat_50
os.system('python latency_hist_50.py a')
os.system('python latency_hist_50.py b')
os.system('python latency_time_50.py a')
os.system('python latency_time_50.py b')
import power
