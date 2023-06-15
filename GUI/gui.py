<<<<<<< HEAD
import tkinter as tk

ventana = tk.Tk()

#Titulo de la ventana
ventana.title("Grafico")

#Tamaño de la ventana
ventana.geometry("800x500")

texto = tk.Label(ventana, text = "Hola mundo")
texto.pack()

ventana.resizable(0,0)

def imprimir():
    label1 = tk.Label(ventana, text="Imprimiendo....")
    label1.pack()

boton1 = tk.Button(ventana, text="Imprimir", command=imprimir)
boton1.pack()

ventana.mainloop()
=======
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon
import os

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Control de LED")
        self.setGeometry(100, 100, 400, 200)
        
        # Botón para encender el LED
        self.btn_on = QPushButton("Encender LED", self)
        self.btn_on.setGeometry(100, 50, 200, 50)
        self.btn_on.clicked.connect(self.turn_on_led)
        
        # Botón para apagar el LED
        self.btn_off = QPushButton("Apagar LED", self)
        self.btn_off.setGeometry(100, 120, 200, 50)
        self.btn_off.clicked.connect(self.turn_off_led)
        
        self.show()
    
    def turn_on_led(self):
        os.system("echo 1 > /dev/etx_device")  # Comando para encender el LED
        
    def turn_off_led(self):
        os.system("echo 0 > /dev/etx_device")  # Comando para apagar el LED

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    sys.exit(app.exec_())
>>>>>>> 36f9778651c268bd42743a6025fdfbf8f0d1e5ae
