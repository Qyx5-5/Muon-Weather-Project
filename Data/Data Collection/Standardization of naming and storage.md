# Standardization of naming and storage
## Txt Data Collection
```python
def collect_data():
    serial_data = read_serial_data()
    weather_info, air_quality_info, city_info = fetch_data_from_api()

    data_str = "==============================\n"
    data_str += f"Hourly Flux: {serial_data}\n"

    if weather_info and air_quality_info and city_info:
        current_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        data_str += f"Time: {current_time}\n"
        data_str += f"City Name: {city_info['location'][0]['name']}\n"
        data_str += f"Location ID: {city_info['location'][0]['id']}\n"
        data_str += f"Temperature: {weather_info['now']['temp']}°C\n"
        data_str += f"Weather: {weather_info['now']['text']}\n"
        data_str += f"Wind Direction: {weather_info['now']['windDir']}\n"
        data_str += f"Wind Speed: {weather_info['now']['windSpeed']} km/h\n"
        data_str += f"Humidity: {weather_info['now']['humidity']}%\n"
        data_str += f"Pressure: {weather_info['now']['pressure']} hPa\n"
        data_str += f"Visibility: {weather_info['now']['vis']} km\n"
        data_str += f"AQI: {air_quality_info['now']['aqi']}\n"
        data_str += f"AQI Level: {air_quality_info['now']['level']}\n"
        data_str += f"Primary Pollutant: {air_quality_info['now']['primary']}\n"
        data_str += f"PM2.5: {air_quality_info['now']['pm2p5']}\n"
        data_str += f"PM10: {air_quality_info['now']['pm10']}\n"
        data_str += f"NO2: {air_quality_info['now']['no2']}\n"
        data_str += f"SO2: {air_quality_info['now']['so2']}\n"
        data_str += f"CO: {air_quality_info['now']['co']}\n"
        data_str += f"O3: {air_quality_info['now']['o3']}\n"
    else:
        logging.warning("Data collection failed for this iteration.")
        data_str += "Data collection failed."

    data_str += "==============================\n"
    return data_str
```
## Document Naming Convention
Local document naming should follow this convention:
>
    data/
        raw/
		    Location_id_name_1/
			    Machine1/
				    date.txt
			    Machine2/
		    Location_id_name_2/
			    Machine1/
			    Machine2/
        csv/	
            Location_id_name_1/
                Machine1/
                    machine_id.csv
                Machine2/
            Location_id_name_2/
                Machine1/
'Location_id' is acquired from the API.

We may have multiple machines under the same location folders. 'Machine1', 'Machine2' are names of machines, the selection of names can be quite arbitary. But it should be simple, like 'Jason' or something else

Also, we want to assign each machine a unique ID, which is 'machine_id'. It is defined as 'Location_id + Machine1'. Machine1 is its name, like 'Jason' or something else.
## Txt Naming Convention
For a specific machine, we have the following naming convention.

As what was mentioned above, the name of our txt should be 'date.txt'.

```python
def write2txt(input):
    with open(f'./raw_data/{datetime.date.today()}','a') as data_today:
        data_today.write(input)
```
