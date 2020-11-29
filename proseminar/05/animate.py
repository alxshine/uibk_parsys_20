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

upper_coord_bound = 100
lower_coord_bound = 0

data = np.fromfile('stars.dat', dtype=star_type)
df = pd.DataFrame.from_records(data)

fig = px.scatter(df, x="x", y="y",
                 animation_frame="t",
                 range_x=[lower_coord_bound, upper_coord_bound],
                 range_y=[lower_coord_bound, upper_coord_bound],
                 size="m")
fig.show()
