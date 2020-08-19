import sys
import os
import tensorflow as tf
from tensorflow.python.keras.models import load_model, Model, model_from_json
from custus_utilities import custusVolume
from livermask.livermask import get_model, data_pretransform, data_predict, data_posttransform, morph_postprocess

import numpy as np
from scipy.ndimage import zoom

# Remove TF warnings?
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
tf.compat.v1.logging.set_verbosity(tf.compat.v1.logging.ERROR)

# Limit us to using a single GPU (GPU 1)
os.environ['CUDA_DEVICE_ORDER'] = "PCI_BUS_ID"
os.environ['CUDA_VISIBLE_DEVICES'] = "1"

# Allow growth - don't use all GPU memory
config = tf.ConfigProto()
config.gpu_options.allow_growth = True
session = tf.Session(config=config)

# Constants and input variables
n_argin_expected = 2  # Expect input and output volume paths
liver_model_path = 'liver_model.h5'
vessels_model_path = 'liver_vessels_model_3D.hd5'
#vessels_model_path = 'model3D_cropped.hd5'
img_size = 512
input_image_path = ''
output_image_path = ''

# Get liver segmentation model
if not os.path.exists(liver_model_path):
    print('Downloading liver model file')
    get_model(liver_model_path)
#else:
#    print('Liver model file exists')

if len(sys.argv) > n_argin_expected:  # command string is sys.argv[0]
  input_image_path = sys.argv[1] # First argument should always be input volume
  output_image_path = sys.argv[2] # Second argument should always be destination volume file
else:
  print('Too few arguments, script aborted.')
  exit(1)  # TODO: Find proper exit code

input_volume = custusVolume(input_image_path)
input_volume.load_volume()

#data = data_pretransform(data, img_size)
#predicted_output = model.predict(data)
#data = data_posttransform(data, curr_shape, img_size)
#data = morph_postprocess(data)


# Pre process
data = input_volume.get_array()
curr_shape = data.shape  # Remember original shape
print("curr_shape: ", curr_shape)

if data.min() >= 0:  # TODO: make a better method to ensure CT-values in Hunsfield units
    data_shift = 1024.0
    print('Data shift: ', data_shift)
    data = data - data_shift

orig_data_without_pretransform = data.copy()
data = data_pretransform(data, img_size, intensity_clipping_range = [-150, 250])

# Predict liver
# load liver model
liver_model = load_model(liver_model_path, compile=False)
#liver_model.summary()

data = np.expand_dims(data, axis=0) # Need to change shape to use liver model
print("--- Segmenting liver ---")
data = data_predict(data, liver_model, threshold = 0.4)
data = np.squeeze(data, axis=0) # Changing shape back
del liver_model

# Post process
data = morph_postprocess(data) # Remove small objects, and fill holes
liver_mask = data.copy()

# Save liver segmentation
# TODO: Return liver segmentation to CustusX in addition to vessels
data = data_posttransform(data, curr_shape, img_size)
input_volume.save_volume(data, output_image_path + '_liver')

### Starting vessel
data = data_pretransform(orig_data_without_pretransform, img_size, intensity_clipping_range = [0, 400])

# Apply liver as mask
data = data * liver_mask
del orig_data_without_pretransform

# For networks trained in 256x256 images
# resize to fixed size (256x256 image plane)
#img_size = 256
tmp_shape = data.shape
data = zoom(data,
                  zoom=[img_size / tmp_shape[0],
                        img_size / tmp_shape[1],
                        1.0],
                  order=1)

# load vessel model
vessels_model = load_model(vessels_model_path, compile=False)

# Predict vessels
print("--- Segmenting liver vessels ---")

#Create 3D slabs from 3D volume
slab_size = 16
stride = 8
counts = int(np.ceil(data.shape[2] / stride))

combined_data = np.zeros((img_size, img_size, data.shape[2]), dtype=np.float32)

print("Prosessing ", counts, " slabs")
for count in range(counts):
  print("slab: ", count)
  tmp = np.zeros((img_size, img_size, slab_size, 1), dtype=np.float32)
  data_slab = data[:, :, int(count * stride):int((count + 1) * stride)]
  tmp[:, :, :data_slab.shape[-1], 0] = data_slab
  data_slab = tmp.copy()

  # filter away CT slices that are masked
  if np.count_nonzero(data_slab) == 0:
    print("Empty slab: ", count)
    #continue

  data_slab = data_predict(data_slab, vessels_model, threshold = 0.4)
  #print("data_slab: ", data_slab.shape)

#  if count == 0:

    #tmp = np.zeros((img_size, img_size, data.shape[2]), dtype=np.float32)
    #print("tmp: ", tmp.shape)
#    combined_data[:, :, 0:data_slab.shape[2]] = np.squeeze(data_slab, axis=3)
    #print("tmp: ", tmp.shape)
    #combined_data = tmp.copy()

    #combined_data = data_slab.copy()
#    print("combined_data: ", combined_data.shape)
    #combined_data = np.squeeze(combined_data, axis=3) # Changing shape back
#  else:
    #print("skipping slab: ", count)

  start_z = (count*stride)
  stop_z = (count*stride)+data_slab.shape[2]
  slab_z = slab_size
  #print("copy range from slab: ", start_z, stop_z)
  if stop_z > (combined_data.shape[2]):
    slab_z = slab_size - (stop_z - combined_data.shape[2])
    stop_z = combined_data.shape[2]
  #print("modified: ", start_z, stop_z, slab_z)
  combined_data[:, :, start_z:stop_z] = np.squeeze(data_slab, axis=3)[:, :, 0:slab_z]
  #print("combined_data: ", combined_data.shape)

#    continue

#    for index in range(stride, slab_size):
#      if(combined_data.shape[2] < data.shape[2]):
#        data_slice = data_slab[:, :, index]

#        if np.count_nonzero(data_slab) == 0:
#          print("Empty slice: ", index)

        #print("data_slice: ", data_slice.shape)
#        combined_data = np.concatenate((combined_data, data_slice), axis = 2)
    #print("combined_data: ", combined_data.shape)

data=combined_data
print(data.shape)
#data = np.squeeze(data, axis=3) # Changing shape back
#print(data.shape)


# Apply liver mask on vessels as well
#data = data_predict(data, vessels_model, threshold = 0.2)
#data = data * liver_mask
del liver_mask


# Post process
data = data_posttransform(data, curr_shape, img_size)


# Update volume and save
#input_volume.save_volume(data, output_image_path + '_vessels')
input_volume.save_volume(data, output_image_path)
