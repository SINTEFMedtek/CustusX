#! /Users/torgriml/CustusS_settings/profiles/Laboratory/filter_scripts/scripts/python_liversegment/venv/bin/python -u
# NB! Need to install a venv locally, and add full path to the environment in the shebang line above.

import sys
import os
import numpy as np
from custus_utilities import custusVolume, simpleView
from livermask.livermask import get_model, data_pretransform, data_predict, data_posttransform, morph_postprocess
from tensorflow.python.keras.models import load_model

debug = True  # Print extra info


# Constants and input variables
n_argin_expected = 2  # Expect input and output volume paths
model_path = 'data/model.h5'
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
data = data_pretransform(data, img_size)

# Predict
data = data_predict(data, model)

# Post process
data = data_posttransform(data, curr_shape, img_size)
data = morph_postprocess(data)

# Update volume and save
input_volume.save_volume(data, output_image_path)


