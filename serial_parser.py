import serial
import sys
import re

COM_PORT = "COM6"
BAUD_RATE = 115200
NUM_DATA = 6
data_array = [0] * NUM_DATA

print("Starting serial parser...")

ser = serial.Serial (COM_PORT, BAUD_RATE)

while True:
    cc=str(ser.readline())
    #include data, timestamp, id?
    match_obj = re.search("Data:\s+(\d+),\s+(\d+),\s+(\d+),\s+(\d+),\s+(\d+),\s+(\d+)", cc) ##Finne en bedre måte å gjøre dette på?
    if match_obj is not None:
        for n in range(1, NUM_DATA+1):
            data_array[n-1] = match_obj.group(n)
        print(data_array) # Evt. send til database