import serial
import sys
import re

# from database_models import insert_measurement, SENSOR_HUMIDITY, SENSOR_TEMPERATURE

import serial.tools.list_ports

print ('Search ports...')
ports = list(serial.tools.list_ports.comports())

for p in ports:
    print ('-- Find ports --')
    print (p.description)
    print (p.hwid)

COM_PORT = p.device #"COM6"
BAUD_RATE = 115200
NUM_DATA = 3
data_array = [0] * NUM_DATA

print("Starting serial parser...")

ser = serial.Serial (COM_PORT, BAUD_RATE)

while True:
    cc=str(ser.readline())
    match_obj = re.search("Data:\s+(\d+),\s+(\d+),\s+(\d+)", cc) ##Finne en bedre måte å gjøre dette på?
    if match_obj is not None:
        # humidity, temperature = match_obj.groups()
        for n in range(1, NUM_DATA+1):
            data_array[n-1] = match_obj.group(n)
        print(data_array) # Evt. send til database
        # insert_measurement(humidity, SENSOR_HUMIDITY)
        # insert_measurement(temperature, SENSOR_TEMPERATURE)
