#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import sys
import os
import numpy as np
from pathlib import Path
import pandas as pd
from bokeh.plotting import figure, output_file, save
from bokeh.models import ColumnDataSource, Legend, Title
from bokeh.io import curdoc
from bokeh.models.tools import HoverTool
from bokeh.transform import dodge
from bokeh.palettes import Category20
from bokeh.layouts import column

def get_latencies(file_path):
	dfx = pd.read_csv(file_path)
	dfx = dfx.loc[dfx['thread'] == 0]
	dfx = dfx[['time', 'latency']]
	dfx['time'] = dfx['time'] / 1000000000
	col_latency = Path(file_path).stem
	col_time = "time_" + col_latency
	dfx.rename(columns={'time': col_time, 'latency': col_latency}, inplace=True)
	return dfx

dstat = pd.read_csv('results\\logtest1_t4_l1500_w100000_d30000_latency_0x358db4ae.csv')

name_string = 'latency_time_50'
if len(sys.argv) > 1 :
	if str(sys.argv[1]) == 'a' :
		dstat = dstat[((dstat['latency_median'] > 100.0) & (dstat['latency_median'] <= 10000.0))]
		name_string += '_a'
	elif str(sys.argv[1]) == 'b' :
		dstat = dstat[dstat['latency_median'] > 10000.0]
		name_string += '_b'

x_max = dstat['latency_99th'].max()
x_min = dstat['latency_min'].min()

dstat['file'] = 'results\\logtest1_t4_l1500_w100000_d30000_latency_0x358db4ae\\' + dstat['lib_name'] + '_' + dstat['lib_ver']  + '_' + dstat['formatter'] + '_' + dstat['sink'] + '_' + dstat['build'] + '.csv'
dstat = dstat[['file', 'latency_median']]
dstat.sort_values(by=['latency_median'], inplace=True)
file_list0 = dstat["file"].unique()
file_list1 = [ f.path for f in os.scandir("results\\logtest1_t4_l1500_w100000_d30000_latency_0x358db4ae") if f.is_file() ]
file_list = [ f for f in file_list0 if f in file_list1]

dfs = []
for f in file_list:
	dfs.append(get_latencies(f))

y_max = 0
for d in dfs:
	yy_max = d.tail(1).iloc[0,0]
	if yy_max > y_max:
		y_max = yy_max

list_size = len(dfs)

df = dfs[0]
ind = 1
while ind < list_size :
	df = df.join(dfs[ind])
	ind = ind + 1

cols_list = df.columns.tolist()
lib_names = cols_list[1::2]
lib_times = cols_list[0::2]


output_file(filename=name_string +'.html', title=name_string)

curdoc().theme = 'dark_minimal'
source = ColumnDataSource(df)

p = figure(x_range=[-1, y_max + 1], y_range=[x_min, x_max], y_axis_type = "log", width=1200, height=600)
p.add_layout(Legend(orientation="horizontal"), 'below')

for ind in reversed(range(0, len(lib_names))):
	legend_label = lib_names[ind]
	legend_label = '_'.join(legend_label.split("_")[:2])
	color=Category20[20][ind%20]
	p.line(x=lib_times[ind], y=lib_names[ind],
			 source=source,
			 line_width=1,
			 line_color=color,
			 legend_label = legend_label)

p.title.text_font_size = '16pt'
p.title.align = 'center'
p.title.text ='Latency time graphs (50 logs/sec; thread 0)'
p.xaxis.axis_label = 'time (second)'
p.yaxis.axis_label = 'latency (nanosecond)'
p.xaxis.ticker.num_minor_ticks=10
p.xaxis.ticker.desired_num_ticks=10
p.yaxis.ticker.num_minor_ticks=10
p.yaxis.ticker.desired_num_ticks=10
p.xgrid.grid_line_color = '#909090'
p.xgrid.minor_grid_line_color = '#303030'
p.ygrid.grid_line_color = '#909090'
p.ygrid.minor_grid_line_color = '#303030'
p.legend.ncols=8
p.legend.items = list(reversed(p.legend[0].items))
p.legend.click_policy='hide'

p.add_layout(Title(text='benchmark:    logbench ./bin -o ./results -c 10000 -b ./ramdrive -t 4 -p 1 5 -l 1500 -w 100000 -d 30000 --dropped\nlog call:           LOG("Thr: {} Log_n: {} Time: {} {} {}", (int) thread, (uint64_t) log_no, (uint64_t) nanosec, double(123.456789), <float>::infinity());\noutput:            2024-08-04 17:58:43.734915 +0200 INFO .../logger.hpp:42 Thr: 1 Log_n: 1 Time: 1202671383528328 123.456789 inf', align="left", text_color='#909090'), "below")

save(p)
