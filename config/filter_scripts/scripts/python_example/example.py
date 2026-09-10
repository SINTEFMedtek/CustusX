import sys
import tkinter as tk
from tkinter import filedialog
from custus_utilities import custusVolume


print("Example script is running. ")
print("Python version: ", sys.version)
print('Number of arguments:', len(sys.argv), 'arguments.')
print('Argument List:', str(sys.argv))

# Input dialog example:
root = tk.Tk()
root.withdraw()
data_path = filedialog.askdirectory(title = "Choose data folder")

# Duplcate input volume:
input_image_path = sys.argv[1] # First argument should always be input volume
output_image_path = sys.argv[2] # Second argument should always be destination volume file
input_volume = custusVolume(input_image_path)
input_volume.duplicate(output_image_path)
