import sys
import os
import numpy as np
from custus_utilities import custusVolume, simpleView
from livermask.livermask import get_model, data_pretransform, data_predict, data_posttransform, morph_postprocess
from tensorflow.python.keras.models import load_model

debug = True  # Print extra info

# Constants and input variables
n_argin_expected = 2  # Expect input and output volume paths
model_path = 'networks/liver_model.h5'
img_size = 512
input_image_path = ''
output_image_path = ''

if debug == True:
    # Print some initial info for debug
    print("Segmentation script is running. ")
    print("Python version: ", sys.version)
    print('Number of arguments:', len(sys.argv), 'arguments.')
    # print('Argument List:', str(sys.argv))

# Get input arguments
if len(sys.argv) > n_argin_expected:  # command string is sys.argv[0]
    input_image_path = sys.argv[1]  # First argument should always be input volume
    output_image_path = sys.argv[2]  # Second argument should always be destination volume file
else:
    print('Too few arguments, script aborted.')
    exit(1)  # TODO: Find proper exit code

# Load input volume:
input_volume = custusVolume(input_image_path)
input_volume.load_volume()

# Get segmentation model
if not os.path.exists(model_path):
    print('Downloading model file')
    get_model(model_path)
else:
    print('Model file exists')

# load model
model = load_model(model_path, compile=False)

# Pre process
data = input_volume.get_array()
curr_shape = data.shape  # Remember original shape

if data.min() >= 0:  # TODO: make a better method to ensure CT-values in Hunsfield units
    data_shift = 1024.0
    print('Data shift: ', data_shift)
    data = data - data_shift

data = data_pretransform(data, img_size)

# Predict
data = np.expand_dims(data, axis=0) # Need to change shape to use liver model
data = data_predict(data, model)
data = np.squeeze(data, axis=0) # Changing shape back

# Post process
data = data_posttransform(data, curr_shape, img_size)
data = morph_postprocess(data)

# Update volume and save
input_volume.save_volume(data, output_image_path)


