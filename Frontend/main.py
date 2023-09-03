import streamlit as st
import pandas as pd
from PIL import Image
import plotly.express as px
from influxdb import InfluxDBClient
from influxdb_client import InfluxDBClient
import random 



url = 'http://192.168.254.60:8086'
token = 'vHGp8PcbtpTQVmlplzQMecvZdfpSJcUNT1wOvBXXRyOzeWmW0AsOQMWMRijSoEKuwxlURtrhael_rAD9qfPUMw=='
org = 'ad3830e63761906d'
bucket = 'nikolaus'

data = {'Latitude': [],
        'Longitude': [],
        'percent': [],
        }
fakedata = {
   'Latitude': [43.474960, 43.469070, 43.493980],
   'Longitude': [-80.529678, -80.575690, -80.528820],
   'percent': [random.randint(0, 100) for placeholder in range(3)]
   
}
#refresh 5s delay 
with InfluxDBClient(url=url, token=token, org=org) as client:
    query_api = client.query_api()

    tables = query_api.query('from(bucket: "nikolaus") |> range(start: -2s)')

    for table in tables:
        for record in table.records:
            field = record.get_field()
            value = record.get_value()
            
            if field == 'Latitude':
               data["Latitude"] += [value] 
            elif field == 'Longitude':
               data["Longitude"] += [value]
            elif field == 'percent':
               data["percent"].append(int(value * 100))
                           
            

print(data)
df = pd.DataFrame(data)

df['combinedtext'] = (df['percent'].astype(str) + '%')




with open('stream.css') as f:
  st.markdown(f'<style>{f.read()}</style>', unsafe_allow_html=True)

#Points size
pointcolor = 'yellow'
st.title('WasteOverflow')

image = Image.open('images/boxlogo.png')

st.image(image, width=40)
# use px.scatter_mapbox to 
fig = px.scatter_mapbox(
    df,
    lat="Latitude",
    lon="Longitude",
    text="combinedtext",
    zoom=12,
    color_discrete_sequence=[pointcolor]
)

fig.update_traces(
  textfont=dict(family="Switzer", color="white", size=10)
)

fig.update_traces(
    marker=dict(
        size=25,  # Set the size of the markers
        sizemode="diameter",  # "diameter" sets the marker size as a diameter
        sizemin=5,  # Minimum marker size
    )
)
# Set the Mapbox token
fig.update_layout(
  mapbox=dict(accesstoken="pk.eyJ1Ijoic2Zpc2NobWUiLCJhIjoiY2xtMW5jdG40MzVwdjNrcGlkYTQ2eGY1eCJ9.kNwZdRqcAfU4lhIQn_7jnQ"),
  width=600,
  height=450,
                  )


if st.button("Refresh"):
  st.markdown("<script type='text/javascript'>window.location.reload(true);</script>", unsafe_allow_html=True)

if df['percent'].max() < 80:
    pointcolor = 'green'
else:
    pointcolor = 'red'

# Update the color of the map markers
fig.update_traces(marker=dict(color=pointcolor))
   

# Display the map
st.plotly_chart(fig)
