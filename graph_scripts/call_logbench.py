#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import subprocess

def logbench(folder, parameters) :
	to_run = "logbench "
	to_run += folder + ' ' + parameters
	subprocess.run(to_run.split())
