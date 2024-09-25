#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import pandas as pd
from bokeh.plotting import figure, output_file, save
from bokeh.models import ColumnDataSource, Title
from bokeh.io import curdoc
from bokeh.models.tools import HoverTool

output_file(filename='throughput.html', title='throughput')

df = pd.read_csv('results/logtest1_t4_l100000_w0_d0_0x358db4ae.csv')
df = df[df['lib_name'] != 'noop']
df['name'] = df['lib_name'] + '_' + df['lib_ver']
df['throughput'] =  round(df['throughput'] / 1000, 2)
df = df[['name', 'throughput']]
df.sort_values(by=['throughput'], ascending=False , inplace=True)

curdoc().theme = 'dark_minimal'
source = ColumnDataSource(df)

p = figure(x_range=source.data['name'], width=1200, height=400)
p.vbar(x='name', top='throughput', bottom = 0, source=source, width=0.70, color='green', fill_alpha=0.7, line_color=None)
p.title.text_font_size = '16pt'
p.title.align = 'center'
p.title.text ='Max throughput of log system'
p.yaxis.axis_label = 'Throughput (1000 lines / second)'
p.xaxis.major_label_orientation = 0.5
p.xgrid.grid_line_color = None	#remove the x grid lines
p.y_range.start = 0
p.ygrid.grid_line_color = '#a0a0a0'
p.ygrid.minor_grid_line_color = '#303030'

p.add_layout(Title(text='benchmark:    logbench ./bin -o ./results -c 10000 -b ./ramdrive -t 4 -p 1 5 -l 100000 -w 0 -d 0 --dropped\nlog call:           LOG("Thr: {} Log_n: {} Time: {} {} {}", (int) thread, (uint64_t) log_no, (uint64_t) nanosec, double(123.456789), <float>::infinity());\noutput:            2024-08-04 17:58:43.734915 +0200 INFO .../logger.hpp:42 Thr: 1 Log_n: 1 Time: 1202671383528328 123.456789 inf', align="left", text_color='#909090'), "below")
p.add_tools(HoverTool(tooltips="@throughput", mode="vline", anchor='top_center'))

save(p)
