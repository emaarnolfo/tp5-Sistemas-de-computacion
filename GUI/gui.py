import matplotlib.pyplot as plt
import time
import os

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk

# Valores para el sensor ultrasónico
x_values_dist = []
y_values_dist = []

# Valores para el Joystick analogico
x_values_joy = []
y_values_joy = []

valor = 0

while valor <= 10000:
    y_values_dist.append(valor)
    #y_values_dist.append(0)
    x_values_dist.append(valor)
    y_values_joy.append(valor)
    #y_values_joy.append(0)
    x_values_joy.append(valor)
    valor += 20

def update_graph_dist():
    indice = 0
    while (1):
    #    with open('/dev/ultrasonic', 'rb') as file:
    #		data = file.read()
        
    #	trip = int.from_bytes(data, 'little')
    #	secs = trip * 1e-6 / 2
    #	dist = 340 * secs
    #	y_values[indice%len(x_values)] = dist * 100
        #print(f'Total roundtrip took {trip} us, this gives us a distance of {dist * 100:.2f} cm')
        
        plt.clf()	#Limpiar el gráfico anterior
        plt.plot(x_values_dist, y_values_dist)
        plt.xlabel('Eje X')
        plt.ylabel('Eje Y')
        plt.title('Grafico de Datos')
        plt.ylim(0,60)
        plt.pause(0.001)		# Pausa para lograr la actualizacion del grafico
        indice += 1

        if not plt.fignum_exists(plt.figure):  # Verificar si la figura ha sido cerrada
            break
        
def update_graph_joy():
    indice = 0
    while (1):
    #	with open('/dev/joystick', 'rb') as file:
    #		data = file.read()
            #y_values[indice%len(x_values)] = int.from_bytes(data, byteorder='little', signed=False)

            #print(f'value_joy: {value_joy}') 
            
            plt.clf()	#Limpiar el gráfico anterior
            plt.plot(x_values_joy, y_values_joy)
            plt.xlabel('Eje X')
            plt.ylabel('Eje Y')
            plt.title('Grafico de Datos')
            plt.ylim(0,650)
            plt.pause(0.001)		# Pausa para lograr la actualizacion del grafico
            indice += 1

            if not plt.fignum_exists(plt.figure):  # Verificar si la figura ha sido cerrada
                 break

# Crear ventana de la interfaz
window = tk.Tk()
window.title("Interfaz de Gráficos")

# Crear figura de Matplotlib
fig = plt.figure()

# Crear lienzo de Matplotlib para mostrar en la interfaz
canvas = FigureCanvasTkAgg(fig, master=window)
canvas.get_tk_widget().pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

# Contenedor para los botones
button_frame = tk.Frame(window, padx=10, pady=10)
button_frame.pack(side=tk.LEFT)

# Botón para mostrar el gráfico del sensor ultrasonico
button_graph1 = tk.Button(button_frame, text="Sensor ultrasónico", command=lambda: update_graph_dist(), pady=10)
button_graph1.pack(pady=20)

# Botón para mostrar el gráfico 2
button_graph2 = tk.Button(button_frame, text="Joystick analógico", command=lambda: update_graph_joy(), pady=10)
button_graph2.pack(pady=20)

# Configurar el modo interactivo de Matplotlib
plt.ion()

# Función para cerrar la ventana y detener la actualización del gráfico
def close_window():
    plt.ioff()
    window.destroy()

# Configurar cierre de ventana
window.protocol("WM_DELETE_WINDOW", close_window)

# Iniciar bucle de la ventana
while True:
    window.update()
    if not window.winfo_exists():  # Verificar si la ventana ha sido cerrada
        break