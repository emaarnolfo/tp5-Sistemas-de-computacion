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