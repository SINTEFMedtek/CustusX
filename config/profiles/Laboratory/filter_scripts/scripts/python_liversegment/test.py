

import sys
import os
import shutil
from custus_utilities import custusVolume
from livermask.livermask import get_model
#from tensorflow.python.keras.models import load_model

#import tkinter as tk
#from tkinter import filedialog

print("Python version: ", sys.version)

model_path = 'data/model.h5'

# Get segmentation model
if not os.path.exists(model_path):
    get_model(model_path)
else:
    print('Model file found')