import numpy as np
import os, sys
import h5py
from tqdm import tqdm
import nibabel as nib
from nibabel.processing import resample_to_output, resample_from_to
from scipy.ndimage import zoom
from tensorflow.python.keras.models import load_model
import gdown
from skimage.morphology import remove_small_holes, binary_dilation, binary_erosion, ball
from skimage.measure import label, regionprops
import warnings
import matplotlib.pyplot as plt

warnings.filterwarnings('ignore', '.*output shape of zoom.*')
debug = True  # Print extra info


class simpleView():
    def __init__(self, image_pixels):
        self.image = image_pixels

    def plot_pixels(self):
        plt.imshow(self.image, cmap='gray')
        plt.show()


def intensity_normalization(volume, intensity_clipping_range):
    result = np.copy(volume)

    result[volume < intensity_clipping_range[0]] = intensity_clipping_range[0]
    result[volume > intensity_clipping_range[1]] = intensity_clipping_range[1]

    min_val = np.amin(result)
    max_val = np.amax(result)
    if (max_val - min_val) != 0:
        result = (result - min_val) / (max_val - min_val)

    return result


def post_process(pred):
    return pred


def get_model(output='model.h5'):
    url = "https://drive.google.com/uc?id=181VE-FiqZ2z7xY30LK9GIvLeEEJW0YF-"
    md5 = "ef5a6dfb794b39bea03f5496a9a49d4d"
    gdown.cached_download(url, output, md5=md5)  # , postprocess=gdown.extractall)


def data_pretransform(data, img_size=512, intensity_clipping_range = [-150, 250]):
    # resize
    data = zoom(data, [1.0, img_size / data.shape[1], img_size / data.shape[2]], order=1)

    # intensity normalization
    #intensity_clipping_range = [-150, 250]  # HU clipping limits (Pravdaray's configs)
    data = intensity_normalization(volume=data, intensity_clipping_range=intensity_clipping_range)

    # fix orientation
    data = np.swapaxes(data, 0, 2)
    data = np.rot90(data, k=-1, axes=(0, 1))
    data = np.flip(data, axis=2)
    return data


def data_predict(data, model, threshold = 0.5):
    # predict on data
    pred = np.zeros_like(data).astype(np.float32)
    for i in range(data.shape[-1]):
        print('Predicting slice ', i, ' of ', data.shape[-1])
        pred[..., i] = model.predict(np.expand_dims(np.expand_dims(data[..., i], axis=0), axis=-1))[0, ..., 1]

    # threshold
    pred = (pred >= threshold).astype(int)
    return pred


def data_posttransform(data, curr_shape, img_size):
    # fix orientation back
    data = np.flip(data, axis=2)
    data = np.rot90(data, k=1, axes=(0, 1))
    data = np.swapaxes(data, 0, 2)

    # resize back
    data = zoom(data, [1.0, curr_shape[1] / img_size, curr_shape[2] / img_size], order=1)
    data = (data >= 0.5).astype(np.float32)
    return data


def morph_postprocess(data):
    # morphological post-processing
    print("morphological post-processing...")
    if debug:
        print('Data in: ', data.shape)

    ### opening
    # 1) first erode
    data = binary_erosion(data.astype(bool), ball(3)).astype(np.float32)
    if debug:
        print('Data eroded: ', data.shape)

    # 2) keep only largest connected component
    labels = label(data)
    regions = regionprops(labels)
    area_sizes = []
    for region in regions:
        area_sizes.append([region.label, region.area])
    area_sizes = np.array(area_sizes)
    if debug:
        print('area_sizes: ', area_sizes)
    if len(area_sizes > 1):
        tmp = np.zeros_like(data)
        tmp[labels == area_sizes[np.argmax(area_sizes[:, 1]), 0]] = 1
        data = tmp.copy()
        del tmp
    del labels, regions, area_sizes

    # 3) dilate
    data = binary_dilation(data.astype(bool), ball(3))

    ### closing with remove small holes
    data = binary_dilation(data.astype(bool), ball(3))
    data = remove_small_holes(data.astype(bool), area_threshold=0.001 * np.prod(data.shape)).astype(np.float32)
    data = binary_erosion(data.astype(bool), ball(3))

    return data.astype(np.uint8)


def func(path, output):
    cwd = "/".join(os.path.realpath(__file__).replace("\\", "/").split("/")[:-1]) + "/"

    # print(cwd)
    # print(" :) ")

    name = cwd + "model.h5"
    # name = "\.model.h5"

    # get model
    get_model()

    # load model
    model = load_model(name, compile=False)

    print("preprocessing...")
    nib_volume = nib.load(path)
    new_spacing = [1., 1., 1.]
    resampled_volume = resample_to_output(nib_volume, new_spacing, order=1)
    data = resampled_volume.get_data().astype('float32')
    curr_shape = data.shape
    if debug:
        print('Original data shape: ', data.shape)
        print('Min voxel value: ', data.min())
        print('Max voxel value: ', data.max())

    # resize to get (512, 512) output images
    img_size = 512
    data = zoom(data, [img_size / data.shape[0], img_size / data.shape[1], 1.0], order=1)

    # intensity normalization
    intensity_clipping_range = [-150, 250]  # HU clipping limits (Pravdaray's configs)
    data = intensity_normalization(volume=data, intensity_clipping_range=intensity_clipping_range)

    # fix orientation
    data = np.rot90(data, k=1, axes=(0, 1))
    data = np.flip(data, axis=0)
    if debug:
        print('Data for prediction, shape: ', data.shape)

    print("predicting...")
    # predict on data
    pred = np.zeros_like(data).astype(np.float32)
    for i in tqdm(range(data.shape[-1]), "pred: "):
        pred[..., i] = \
        model.predict(np.expand_dims(np.expand_dims(np.expand_dims(data[..., i], axis=0), axis=-1), axis=0))[0, ..., 1]
    del data

    # threshold
    pred = (pred >= 0.4).astype(int)

    # fix orientation back
    pred = np.flip(pred, axis=0)
    pred = np.rot90(pred, k=-1, axes=(0, 1))

    print("resize back...")
    # resize back from 512x512
    pred = zoom(pred, [curr_shape[0] / img_size, curr_shape[1] / img_size, 1.0], order=1)
    pred = (pred >= 0.5).astype(np.float32)

    print("morphological post-processing...")
    # morpological post-processing
    # 1) first erode
    pred = binary_erosion(pred.astype(bool), ball(3)).astype(np.float32)

    # 2) keep only largest connected component
    labels = label(pred)
    regions = regionprops(labels)
    area_sizes = []
    for region in regions:
        area_sizes.append([region.label, region.area])
    area_sizes = np.array(area_sizes)
    tmp = np.zeros_like(pred)
    tmp[labels == area_sizes[np.argmax(area_sizes[:, 1]), 0]] = 1
    pred = tmp.copy()
    del tmp, labels, regions, area_sizes

    # 3) dilate
    pred = binary_dilation(pred.astype(bool), ball(3))

    # 4) remove small holes
    pred = remove_small_holes(pred.astype(bool), area_threshold=0.001 * np.prod(pred.shape)).astype(np.float32)

    print("saving...")
    pred = pred.astype(np.uint8)
    img = nib.Nifti1Image(pred, affine=resampled_volume.affine)
    resampled_lab = resample_from_to(img, nib_volume, order=0)
    nib.save(resampled_lab, output)


def main():
    os.environ["CUDA_VISIBLE_DEVICES"] = "-1"

    # __os.path

    path = sys.argv[1]
    output = sys.argv[2]
    # output = sys.argv[3]

    func(path, output)


if __name__ == "__main__":
    main()
