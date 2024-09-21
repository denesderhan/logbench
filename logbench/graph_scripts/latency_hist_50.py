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
	dfx = dfx[['time', 'latency']]
	dfx['time'] = dfx['time'] / 1000000000
	dfx.sort_values(by=['time'], inplace=True)
	col_latency = Path(file_path).stem
	col_time = "time_" + col_latency
	dfx.rename(columns={'time': col_time, 'latency': col_latency}, inplace=True)
	return dfx

dstat = pd.read_csv('results\\logtest1_t4_l1500_w100000_d30000_latency_0x358db4ae.csv')

name_string = 'latency_hist_50'
if len(sys.argv) > 1 :
	if str(sys.argv[1]) == 'a' :
		dstat = dstat[((dstat['latency_median'] > 100.0) & (dstat['latency_median'] <= 10000.0))]
		name_string += '_a'
	elif str(sys.argv[1]) == 'b' :
		dstat = dstat[dstat['latency_median'] > 10000.0]
		name_string += '_b'

x_max = dstat['latency_max'].max()
x_min = dstat['latency_min'].min()
x_zoom_max = dstat['latency_90th'].max() * 2
x_zoom_min = x_min

dstat.sort_values(by=['latency_median'], inplace=True)

dstat['file'] = 'results\\logtest1_t4_l1500_w100000_d30000_latency_0x358db4ae\\' + dstat['lib_name'] + '_' + dstat['lib_ver']  + '_' + dstat['formatter'] + '_' + dstat['sink'] + '_' + dstat['build'] + '.csv'
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
if list_size == 0 :
	sys.exit(0)
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

p1 = figure(x_range=(x_zoom_min, x_zoom_max), x_axis_type = "log", y_axis_type = "log", width=1200, height=400)
p1.y_range.start = 0.7
p1.add_layout(Legend(orientation="horizontal"), 'above')
p2 = figure(x_range=p1.x_range, y_range=[y_max, 0], x_axis_type = "log", width=1200, height=350, x_axis_location=None)
p2.add_layout(Legend(orientation="horizontal"), 'below')

p1.title.text_font_size = '16pt'
p1.title.align = 'center'
p1.title.text ='Latency histograms (50 logs/sec)'
p1.xaxis.axis_label = 'latency (nanosecond)'
p1.yaxis.axis_label = 'number of samples'
p2.title.text_font_size = '16pt'
p2.title.align = 'center'
p2.title.text ='Latency time plots'
p2.yaxis.axis_label = 'time (second)'

#p.x_range = Range1d(x_zoom_min, x_zoom_max)

bins = np.logspace(np.log10(x_min), np.log10(x_max*2), 400)

for ind in reversed(range(0, len(lib_names))):
	legend_label = lib_names[ind]
	legend_label = '_'.join(legend_label.split("_")[:2])
	color=Category20[20][ind%20]
	p2.circle(y=lib_times[ind], x=lib_names[ind],
			 source=source,
			 size=2.5,
			 line_color=None,
			 color=color,
			 fill_alpha=0.4,
			 legend_label = legend_label)
	hist, edges = np.histogram(df[lib_names[ind]], density=False, bins=bins)
	hist_df = pd.DataFrame({'x': hist, 
                       'left': edges[:-1], 
                       'right': edges[1:]})
	p1.quad(bottom=0.7, top=hist_df['x'], 
       left=hist_df['left'], right=hist_df['right'], 
       fill_color=color, line_color=color,
	   line_alpha=0.35, fill_alpha=0.7,
	   legend_label=legend_label)

p1.xaxis.ticker.num_minor_ticks=10
p1.yaxis.ticker.num_minor_ticks=5
p1.xgrid.grid_line_color = '#909090'
p1.xgrid.minor_grid_line_color = '#303030'
p1.ygrid.grid_line_color = '#909090'
p1.ygrid.minor_grid_line_color = '#303030'
p2.xgrid.grid_line_color = '#909090'
p2.xgrid.minor_grid_line_color = '#303030'

p1.legend.items = list(reversed(p1.legend[0].items))
p1.legend.ncols=8
p2.legend.items = list(reversed(p2.legend[0].items))
p2.legend.ncols=8
p1.legend.click_policy='hide'
p2.legend.click_policy='hide'

p2.add_layout(Title(text='benchmark:    logbench ./bin -o ./results -c 10000 -b ./ramdrive -t 4 -p 1 5 -l 1500 -w 100000 -d 30000 --dropped\nlog call:           LOG("Thr: {} Log_n: {} Time: {} {} {}", (int) thread, (uint64_t) log_no, (uint64_t) nanosec, double(123.456789), <float>::infinity());\noutput:            2024-08-04 17:58:43,734915 +0200 INFO .../logger.hpp:42 Thr: 1 Log_n: 1 Time: 1202671383528328 123.456789 inf', align="left", text_color='#909090'), "below")

vp = column(p1, p2)

save(vp)
