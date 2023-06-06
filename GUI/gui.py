import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon
import os

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Control de LED")
        self.setGeometry(100, 100, 200, 100)
        
        # Botón para encender el LED
        self.btn_on = QPushButton("Encender LED", self)
        self.btn_on.setGeometry(50, 20, 100, 30)
        self.btn_on.clicked.connect(self.turn_on_led)
        
        # Botón para apagar el LED
        self.btn_off = QPushButton("Apagar LED", self)
        self.btn_off.setGeometry(50, 60, 100, 30)
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
