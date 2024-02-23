""" This script converts all .txt files collected from oscilloscope
to .csv """
# %%
import os
import pandas as pd
def parse_weather_data(file_path):
    # Mapping dictionary for various field namings
    field_mappings = {
        "Total Count": "count",
        "totalcount": "count",
        "City Name": "city",
        "City": "city",
        "Time": "date",
        "Week": "week",
        "Weather": "weather",
        "Temperature": "temperature",
        "AQI": "quality",
        "Air Quality": "quality",        
        "AQI Level": "level",
        "air_level": "level",
        "Humidity": "humidity",
        "air_humidity": "humidity",
        "Pressure": "pressure"
    }

    # Function to extract data from a line
    def extract_data(line):
        line = line.strip()
        for key, value in field_mappings.items():
            if line.lower().startswith(key.lower() + ":"):
                parts = line.split(': ')
                if len(parts) > 1:
                    return value, parts[1].strip()
        return None, None

    data = []
    current_record = {}
    with open(file_path, 'r', encoding='utf-8') as file:
        for line in file:
            field, value = extract_data(line)
            if field:
                current_record[field] = value
            if line.strip() == "==============================":
                if current_record:
                    data.append(current_record)
                    current_record = {}
    return data

# Example usage
folder_path = 'muon_daq_database'  # Replace with the path to your folder

# Function to parse all files in a folder
def parse_folder(folder_path):
    all_data = []
    for filename in os.listdir(folder_path):
        if filename.endswith('.txt'):  # Assuming files are .txt
            file_path = os.path.join(folder_path, filename)
            file_data = parse_weather_data(file_path)
            all_data.extend(file_data)
    return all_data

parsed_data = parse_folder(folder_path)
print(parsed_data)


# %%
# Convert the parsed data to a DataFrame
df = pd.DataFrame(parsed_data)

# Display the DataFrame
print(df)

# %%
# Convert the DataFrame to a CSV file
csv_file_name = 'weather_data.csv'
df.to_csv(csv_file_name, index=False)

print(f"Data exported to CSV file: {csv_file_name}")


