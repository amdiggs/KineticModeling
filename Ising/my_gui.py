import tkinter as tk
import numpy as np


class my_Button():
    def __init__(self, name, param):
        self.win = tk.Tk()
        self.lab = tk.Label(text = name, width = 16, height = 32)
        self.lab.pack()
        self.val = param
        self.up = tk.Button()



if __name__=='__main__':
    B=my_Button("Temp")
    B.win.mainloop()
