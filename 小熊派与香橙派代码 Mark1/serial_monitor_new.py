import os
import requests
import json
import datetime
import logging
import serial
import schedule
import time

# Constants for Serial Communication
SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200
GPIO_PIN = 7

# Initialize Serial Port
def initialize_serial():
    global ser
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
    except serial.SerialException as e:
        logging.error(f"Error opening serial port: {e}")
        exit(1)

# Initialize Logging
def initialize_logging():
    log_filename = f'data_collection_{datetime.date.today()}.log'
    log_dir = '/home/orangepi/python_projects/python/python/datalog/'
    os.makedirs(log_dir, exist_ok=True)
    log_path = os.path.join(log_dir, log_filename)

    for handler in logging.root.handlers[:]:
        logging.root.removeHandler(handler)
    logging.basicConfig(filename=log_path, level=logging.INFO,
                        format='%(asctime)s:%(levelname)s:%(message)s')

# Read Data from Serial Port
def read_serial_data():
    if ser:
        try:
            sertest = serial.Serial(SERIAL_PORT, BAUD_RATE)
            data = sertest.readline().decode('UTF-8').strip()
            logging.info(f"Serial Data: {data}")
            sertest.close()
            return data
        except serial.SerialException as e:
            logging.error(f"Error reading from serial port: {e}")
            return None
    else:
        logging.error("Serial port not initialized.")
        return None

def fetch_data_from_api():
    try:
        # API base URLs
        weather_url = "https://devapi.qweather.com/v7/weather/now"
        air_quality_url = "https://devapi.qweather.com/v7/air/now"
        city_url = "https://geoapi.qweather.com/v2/city/lookup"

        # API parameters
        params = {
            'key': 'your key',  # Replace with your API key
            'location': '101010200'  # Replace with your desired location
        }

        # Request data for weather, air quality, and city information
        weather_response = requests.get(weather_url, params=params)
        air_quality_response = requests.get(air_quality_url, params=params)
        city_response = requests.get(city_url, params=params)

        # Check response status and parse data
        if weather_response.status_code == 200:
            weather_info = weather_response.json()
        else:
            logging.error(f"Weather data fetch failed with status code: {weather_response.status_code}")
            weather_info = None

        if air_quality_response.status_code == 200:
            air_quality_info = air_quality_response.json()
        else:
            logging.error(f"Air quality data fetch failed with status code: {air_quality_response.status_code}")
            air_quality_info = None

        if city_response.status_code == 200:
            city_info = city_response.json()
        else:
            logging.error(f"City data fetch failed with status code: {city_response.status_code}")
            city_info = None

        return weather_info, air_quality_info, city_info

    except requests.RequestException as e:
        logging.error(f"HTTP request exception during data fetch: {e}")
        return None, None, None
    except json.JSONDecodeError as e:
        logging.error(f"JSON decoding error: {e}")
        return None, None, None
    except Exception as e:
        logging.error(f"General exception during data fetch: {e}")
        return None, None, None

# Combine Data Collection Functions
def collect_data():
    serial_data = read_serial_data()
    weather_info, air_quality_info, city_info = fetch_data_from_api()
    # Combine data from serial and API
    data_str = f"Serial Data: {serial_data}\n"
    if weather_info and air_quality_info and city_info:
        data_str += (f"Collect Time: {datetime.datetime.today()}\n")
        data_str += (f"City Name: {city_info['location'][0]['name']}\n" +
                     f"Location ID: {city_info['location'][0]['id']}\n" +
                     f"Temperature: {weather_info['now']['temp']}°C\n" +
                     f"Weather: {weather_info['now']['text']}\n" +
                     f"AQI: {air_quality_info['now']['aqi']}\n" +
                     f"Primary Pollutant: {air_quality_info['now']['primary']}\n")
    else:
        logging.warning("Data collection failed for this iteration.")
        data_str += "Data collection failed."
    return data_str
def write2txt(input):
    with open(f'./raw_data/{datetime.date.today()}','a') as data_today:
        data_today.write(input)
# Main Execution
if __name__ == "__main__":
    # Initialize GPIO
    #GPIO.setmode(GPIO.BOARD)
    #GPIO.setup(GPIO_PIN, GPIO.OUT)
    #GPIO.output(GPIO_PIN, GPIO.LOW)

    # Initialize Logging and Serial
    initialize_logging()
    initialize_serial()

    # Scheduling Tasks
    schedule.every(1).hours.do(lambda: write2txt(collect_data())) # Adjust frequency as needed

    logging.info("Data collection script started.")
    try:
        while True:
            schedule.run_pending()
            time.sleep(1)
    except KeyboardInterrupt:
        logging.info("Script stopped by user.")
    finally:
        if ser:
            ser.close()
        #GPIO.cleanup()    
