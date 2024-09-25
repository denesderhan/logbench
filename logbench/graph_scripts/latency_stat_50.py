#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import pandas as pd
from bokeh.plotting import figure, output_file, save
from bokeh.models import ColumnDataSource, PrintfTickFormatter, LogTicker, Title
from bokeh.io import curdoc
from bokeh.models.tools import HoverTool
from bokeh.models.layouts import TabPanel, Tabs
from bokeh.transform import dodge

output_file(filename='latency_stat_50.html', title='latency_stat_50')

df = pd.read_csv('results/logtest1_t4_l1500_w100000_d30000_latency_0x358db4ae.csv')
df['name'] = df['lib_name'] + '_' + df['lib_ver']
df = df[['name', 'latency_IQR', 'latency_min', 'latency_median', 'latency_99th']]
df = df.sort_values(by='latency_median')
#df = df.sort_values(by='latency_median')

curdoc().theme = 'dark_minimal'
source = ColumnDataSource(df)
lib_names = source.data['name'].tolist()

panels = []
for axis_type in ["log", "linear"]:
	p = figure(x_range=lib_names, y_axis_type = axis_type, width=1200, height=450)
	b1 = p.vbar(x=dodge('name', -0.3, range=p.x_range), top='latency_IQR', bottom = 0.1, source=source, width=0.15, color='magenta', fill_alpha=0.7, line_color=None, legend_label='Interquartile range')
	b2 = p.vbar(x=dodge('name', -0.1, range=p.x_range), top='latency_min', bottom = 0.1, source=source, width=0.15, color='orange', fill_alpha=0.7, line_color=None, legend_label='latency min')
	b3 = p.vbar(x=dodge('name', 0.1, range=p.x_range), top='latency_median', bottom = 0.1, source=source, width=0.15, color='green', fill_alpha=0.7, line_color=None, legend_label='latency median')
	b4 = p.vbar(x=dodge('name', 0.3, range=p.x_range), top='latency_99th', bottom = 0.1, source=source, width=0.15, color='red', fill_alpha=0.7, line_color=None, legend_label='latency 99th')

	p.title.text_font_size = '16pt'
	p.title.align = 'center'
	p.title.text ='Latency statistics at 50 logs/second/thread (4 threads)'
	p.yaxis.axis_label = 'Latency of log call (nanosecond)'
	if (axis_type == "log") :
		p.yaxis.ticker = LogTicker(num_minor_ticks=10)
	p.xaxis.major_label_orientation = 0.5
	p.xgrid.grid_line_color = None	#remove the x grid lines
	p.y_range.start = 0.1
	p.ygrid.grid_line_color = '#a0a0a0'
	p.ygrid.minor_grid_line_color = '#303030'

	p.legend.location = 'top_left'
	p.legend.click_policy='hide'
	p.legend.orientation='horizontal'

	p.add_tools(HoverTool(tooltips="@latency_IQR", renderers=[b1], mode="vline", anchor='top_center'))
	p.add_tools(HoverTool(tooltips="@latency_min", renderers=[b2], mode="vline", anchor='top_center'))
	p.add_tools(HoverTool(tooltips="@latency_median", renderers=[b3], mode="vline", anchor='top_center'))
	p.add_tools(HoverTool(tooltips="@latency_99th", renderers=[b4], mode="vline", anchor='top_center'))

	p.add_layout(Title(text='benchmark:    logbench ./bin -o ./results -c 10000 -b ./ramdrive -t 4 -p 1 5 -l 1500 -w 100000 -d 30000 --latency --dropped\nlog call:           LOG("Thr: {} Log_n: {} Time: {} {} {}", (int) thread, (uint64_t) log_no, (uint64_t) nanosec, double(123.456789), <float>::infinity());\noutput:            2024-08-04 17:58:43.734915 +0200 INFO .../logger.hpp:42 Thr: 1 Log_n: 1 Time: 1202671383528328 123.456789 inf', align="left", text_color='#909090'), "below")
	panel = TabPanel(child=p, title=axis_type)
	panels.append(panel)

tabs = Tabs(tabs=panels)
save(tabs)
