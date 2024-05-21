import serial
import matplotlib.pyplot as plt
import re
import time
import numpy as np

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

power = []
sampling_time = 60

def collect_data(duration):
    start_time = time.time()
    while time.time() - start_time < duration:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line:
                print(f"Received line: '{line}'")
                
                if "Power" in line:
                    power_value = float(re.search(r"Power:\s+([\d.]+) mW", line).group(1))
                    power.append(power_value)
		
        except serial.SerialTimeoutException:
            print("Timeout - no data received")
        except Exception as e:
            print(f"Error: {e}")

def plot_and_save(power):
    plt.figure()
    plt.plot(power, label='Power (mW)')
    
    plt.legend()
    plt.xlabel('Sample')
    plt.ylabel('Value')
    plt.title('Energy consumption')
    plt.savefig('consumption_data_particular.jpg')
    plt.close()
    
    with open('consumption_data_particular.txt', 'w') as f:
        f.write('Sample\tPower (mW)\n')
        for i, value in enumerate(power):
            f.write(f'{i}\t{value}\n')
            
collect_data(sampling_time)
plot_and_save(power)
ser.close()

