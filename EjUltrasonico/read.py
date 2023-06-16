import time 
import os

with open('/dev/ultrasonic', 'rb') as file:
	data = file.read()
	
trip = int.from_bytes(data, 'little')
secs = trip * 1e-6 / 2
dist = 340 * secs
print(f'Total roundtrip took {trip} us, this gives us a distance of {dist * 100:.2f} cm')
