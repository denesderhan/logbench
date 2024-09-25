#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import pandas as pd
from bokeh.plotting import figure, output_file, save
from bokeh.models import ColumnDataSource, PrintfTickFormatter, LogTicker, LabelSet, Title
from bokeh.io import curdoc
from bokeh.models.tools import HoverTool
from bokeh.models.layouts import TabPanel, Tabs
from bokeh.transform import dodge

output_file(filename='power.html', title='power')

df = pd.read_csv('results/logtest1_t4_l0_w0_d100000_0x358db4ae.csv')
df = df[['lib_name', 'lib_ver', 'cpu_cycles']]
df.rename(columns={'cpu_cycles': 'idle'}, inplace=True)

dfx = pd.read_csv('results/logtest1_t4_l50_w0_d100000_0x358db4ae.csv')
dfx = dfx[['lib_name', 'lib_ver', 'cpu_cycles']]
dfx.rename(columns={'cpu_cycles': 'slow'}, inplace=True)
df = pd.merge(df, dfx, how='left', left_on=['lib_name', 'lib_ver'], right_on = ['lib_name', 'lib_ver'])

dfx = pd.read_csv('results/logtest1_t4_l5000_w0_d100000_0x358db4ae.csv')
dfx = dfx[['lib_name', 'lib_ver', 'cpu_cycles']]
dfx.rename(columns={'cpu_cycles': 'medium'}, inplace=True)
df = pd.merge(df, dfx, how='left', left_on=['lib_name', 'lib_ver'], right_on = ['lib_name', 'lib_ver'])

dfx = pd.read_csv('results/logtest1_t4_l50000_w0_d100000_0x358db4ae.csv')
dfx = dfx[['lib_name', 'lib_ver', 'cpu_cycles']]
dfx.rename(columns={'cpu_cycles': 'fast'}, inplace=True)
df = pd.merge(df, dfx, how='left', left_on=['lib_name', 'lib_ver'], right_on = ['lib_name', 'lib_ver'])

noop_idle = df.loc[df['lib_name'] == 'noop', 'idle'].iloc[0]
df['idle'] = (df['idle'] - noop_idle) / 1000000
noop_slow = df.loc[df['lib_name'] == 'noop', 'slow'].iloc[0]
df['slow'] = (df['slow'] - noop_slow) / 1000000
noop_medium = df.loc[df['lib_name'] == 'noop', 'medium'].iloc[0]
df['medium'] = (df['medium'] - noop_medium) / 1000000
noop_fast = df.loc[df['lib_name'] == 'noop', 'fast'].iloc[0]
df['fast'] = (df['fast'] - noop_fast) / 1000000

df = df[df['lib_name'] != 'noop']

df['name'] = df['lib_name'] + '_' + df['lib_ver']
df = df[['name', 'idle', 'slow', 'medium', 'fast']]

df.sort_values(by=['slow'], inplace=True)
curdoc().theme = 'dark_minimal'
source = ColumnDataSource(df)
lib_names = source.data['name'].tolist()

panels = []
for axis_type in [ "log", "linear" ]:
	p = figure(x_range=lib_names, y_axis_type = axis_type, width=1200, height=400)
	b1 = p.vbar(x=dodge('name', -0.3, range=p.x_range), top='idle', bottom = 0.1, source=source, width=0.15, color='green', fill_alpha=0.7, line_color=None, legend_label='0')
	b2 = p.vbar(x=dodge('name', -0.1, range=p.x_range), top='slow', bottom = 0.1, source=source, width=0.15, color='orange', fill_alpha=0.7, line_color=None, legend_label='0.5')
	b3 = p.vbar(x=dodge('name', 0.1, range=p.x_range), top='medium', bottom = 0.1, source=source, width=0.15, color='magenta', fill_alpha=0.7, line_color=None, legend_label='50')
	b4 = p.vbar(x=dodge('name', 0.3, range=p.x_range), top='fast', bottom = 0.1, source=source, width=0.15, color='red', fill_alpha=0.7, line_color=None, legend_label='500')

	p.title.text_font_size = '16pt'
	p.title.align = 'center'
	p.title.text ='CPU cycles used for logging'
	p.yaxis.axis_label = 'Million CPU cycles'
	p.xaxis.major_label_orientation = 0.5
	p.xgrid.grid_line_color = None	#remove the x grid lines
	if (axis_type == "log") :
		p.y_range.start = 1
		p.yaxis.ticker = LogTicker(num_minor_ticks=10)
	p.ygrid.grid_line_color = '#a0a0a0'
	p.ygrid.minor_grid_line_color = '#303030'

	p.add_tools(HoverTool(tooltips="@idle{0.2f}"+" Mcyc", formatters={'@idle{0.2f}' : 'printf'}, renderers=[b1], mode="vline", anchor='top_center'))
	p.add_tools(HoverTool(tooltips="@slow{0.2f}"+" Mcyc", formatters={'@slow{0.2f}' : 'printf'}, renderers=[b2], mode="vline", anchor='top_center'))
	p.add_tools(HoverTool(tooltips="@medium{0.2f}"+" Mcyc", formatters={'@medium{0.2f}' : 'printf'}, renderers=[b3], mode="vline", anchor='top_center'))
	p.add_tools(HoverTool(tooltips="@fast{0.2f}"+" Mcyc", formatters={'@fast{0.2f}' : 'printf'}, renderers=[b4], mode="vline", anchor='top_center'))

	p.legend.location = 'top_left'
	p.legend.click_policy='hide'
	p.legend.orientation='horizontal'
	p.legend.title='Number of log calls /thread /sec (4 thread, 100 second)'
	p.legend.title_text_color='#a0a0a0'

	p.add_layout(Title(text='benchmark:    logbench ./bin -o ./results -c 5000 -b ./ramdrive -t 4 -p 1 5 -l <0|50|5000|50000> -w 0 -d 100000 --dropped\nlog call:           LOG("Thr: {} Log_n: {} Time: {} {} {}", (int) thread, (uint64_t) log_no, (uint64_t) nanosec, double(123.456789), <float>::infinity());\noutput:            2024-08-04 17:58:43.734915 +0200 INFO .../logger.hpp:42 Thr: 1 Log_n: 1 Time: 1202671383528328 123.456789 inf', align="left", text_color='#909090'), "below")
	panel = TabPanel(child=p, title=axis_type)
	panels.append(panel)
	
tabs = Tabs(tabs=panels)
save(tabs)
