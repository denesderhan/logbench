#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).
import numpy as np
import pandas as pd
from bokeh.plotting import figure, output_file, save
from bokeh.models import ColumnDataSource, LabelSet, Title
from bokeh.io import curdoc

output_file(filename='mem.html', title='mem')

df = pd.read_csv('results/logtest1_t4_l100000_w0_d0_0x358db4ae.csv')
base = df.loc[df['lib_name'] == 'noop', 'peak_memory'].iloc[0]
df = df[df['lib_name'] != 'noop']

df['name'] = df['lib_name'] + '_' + df['lib_ver']
df['peak_memory'] = round((df['peak_memory'] - base) / 1048576,2)
df = df[['name', 'peak_memory']]
df.sort_values(by=['peak_memory'], inplace=True)
df['color'] = np.where(df['peak_memory'].le(20), 'green','red')

curdoc().theme = 'dark_minimal'
source = ColumnDataSource(df)

p = figure(x_range=source.data['name'], y_range=[0,50], width=1200, height=400)
p.vbar(x='name', top='peak_memory', bottom = 0, source=source, width=0.70, color='color', fill_alpha=0.7, line_color=None)
p.title.text_font_size = '16pt'
p.title.align = 'center'
p.title.text ='Peak memory used at max throughput'
p.yaxis.axis_label = 'Memory (Mb)'
p.xaxis.major_label_orientation = 0.5
p.xgrid.grid_line_color = None	#remove the x grid lines
p.ygrid.grid_line_color = '#a0a0a0'
p.ygrid.minor_grid_line_color = '#303030'

p.add_layout(Title(text='benchmark:    logbench ./bin -o ./results -c 10000 -b ./ramdrive -t 4 -p 1 5 -l 100000 -w 0 -d 0 --dropped\nlog call:           LOG("Thr: {} Log_n: {} Time: {} {} {}", (int) thread, (uint64_t) log_no, (uint64_t) nanosec, double(123.456789), <float>::infinity());\noutput:            2024-08-04 17:58:43,734915 +0200 INFO .../logger.hpp:42 Thr: 1 Log_n: 1 Time: 1202671383528328 123.456789 inf', align="left", text_color='#909090' ), "below")

label = LabelSet(x='name', y=0.5, text='peak_memory', level='glyph', text_align='center', source=source, text_color='#cccccc')
p.add_layout(label)
save(p)
