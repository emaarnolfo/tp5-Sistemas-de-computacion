import matplotlib.pyplot as plt
import time 
import os

x_values = []
y_values = []
valor = 0

while valor <= 1000:
	#y_values.append(0)
	y_values.append(valor)
	x_values.append(valor)
	valor += 10
	

def read_value(indice):
#	with open('/dev/joystick', 'rb') as file:
#		data = file.read()
		#y_values[indice%len(x_values)] = int.from_bytes(data, byteorder='little', signed=False)

		#print(f'value_joy: {value_joy}') 
		
		plt.clf()	#Limpiar el gráfico anterior
		plt.plot(x_values, y_values)
		plt.xlabel('Eje X')
		plt.ylabel('Eje Y')
		plt.title('Grafico de Datos')
		plt.ylim(0,650)
		plt.pause(0.001)		# Pausa para lograr la actualizacion del grafico
	
# Configurar el modo interactivo de Matplolib
plt.ion()

# Crear la figura
plt.figure()

i = 0
while (1):
	read_value(i)
	i += 1
	if not plt.fignum_exists(plt.figure):  # Verificar si la figura ha sido cerrada
		break
	
plt.ioff()
plt.show()
