import matplotlib.pyplot as plt
import time 
import os

x_values = [100, 200, 300, 400, 500, 600, 700, 800, 900, 1]
y_values = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]


def update_graph(indice):
	with open('/dev/ultrasonic', 'rb') as file:
		data = file.read()
	
	trip = int.from_bytes(data, 'little')
	secs = trip * 1e-6 / 2
	dist = 340 * secs
	y_values[indice%10] = dist * 100
	print(f'Total roundtrip took {trip} us, this gives us a distance of {dist * 100:.2f} cm')
	
	plt.plot(x_values, y_values)
	plt.xlabel('Eje X')
	plt.ylabel('Eje Y')
	plt.title('Grafico de Datos')
	plt.show()
	
for i in range(10):
	update_graph(i)
	
