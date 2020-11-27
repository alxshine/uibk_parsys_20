import plotly.express as px
import pandas as pd
import numpy as np

star_type = np.dtype([
    ('x', 'f4'),
    ('y', 'f4'),
    ('v_x', 'f4'),
    ('v_y', 'f4'),
    ('m', 'f4'),
    ('t', 'i4')])

data = np.fromfile('stars.dat', dtype=star_type)
df = pd.DataFrame.from_records(data)

breakpoint()

fig = px.scatter(df, x="x", y="y",
                 animation_frame="t", range_x=[0, 1000], range_y=[0, 1000])
fig.show()
