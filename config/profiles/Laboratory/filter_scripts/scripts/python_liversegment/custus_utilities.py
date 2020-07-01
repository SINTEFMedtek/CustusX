import os
import shutil
import SimpleITK as sitk
import numpy as np
import matplotlib.pyplot as plt

debug = True  # Print extra info

class custusVolume():
    def __init__(self,volume_path):
        self.volume_path = volume_path
        self.base_path = os.path.dirname(volume_path)

        file_name = os.path.basename(volume_path)
        root_ext = os.path.splitext(file_name)
        self.volume_name = root_ext[0]
        self.volume_type = root_ext[1]
        self.volume = None

    def duplicate(self,new_path):
        new_path = os.path.splitext(new_path)[0]  # Drop extension
        mhd_path = os.path.join(self.base_path,self.volume_name + '.mhd')
        new_mhd_path = new_path + '.mhd'
        shutil.copy(mhd_path, new_mhd_path)

        raw_path = os.path.join(self.base_path, self.volume_name + '.raw')
        new_raw_path = new_path + '.raw'
        shutil.copy(raw_path, new_raw_path)

    def load_volume(self):
        mhd_path = os.path.join(self.base_path, self.volume_name + '.mhd')
        self.volume = sitk.ReadImage(mhd_path)

        if debug == True:
            print('Image origin: ', self.volume.GetOrigin())
            print('Image spacing: ', self.volume.GetSpacing())
            print('Image dtype: ', self.volume.GetPixelIDTypeAsString())

    def resample(self, new_spacing):
        resample = sitk.ResampleImageFilter()
        resample.SetInterpolator = sitk.sitkLinear
        resample.SetOutputDirection = self.volume.GetDirection()
        resample.SetOutputOrigin = self.volume.GetOrigin()
        resample.SetOutputSpacing(new_spacing)

        orig_size = np.array(self.volume.GetSize(), dtype=np.int)
        orig_spacing = self.volume.GetSpacing()
        new_size = orig_size * (orig_spacing / new_spacing)
        new_size = np.ceil(new_size).astype(np.int)  # Image dimensions are in integers
        new_size = [int(s) for s in new_size]
        resample.SetSize(new_size)

        self.volume = resample.Execute(self.volume)

    def get_array(self,dtype='float32'):
        image_array = sitk.GetArrayFromImage(self.volume)
        if debug == True:
            print('Input image type: ', image_array.dtype)
            print('Min voxel value: ', image_array.min())
            print('Max voxel value: ', image_array.max())

        return image_array.astype(dtype)

    def save_volume(self, data, path):
        # Create new volume
        new_volume = sitk.GetImageFromArray(data.astype(np.uint8))  # NB: Only Uint8 for now
        new_volume.CopyInformation(self.volume)  # May want to replace with specific copy of selected items

        # Create path
        base_path = os.path.dirname(path)
        file_name = os.path.basename(path)
        root_ext = os.path.splitext(file_name)
        volume_name = root_ext[0]
        mhd_path = os.path.join(base_path, volume_name + '.mhd')

        # Write to file
        sitk.WriteImage(new_volume, mhd_path)


class simpleView():
    def __init__(self, image_pixels):
        self.image = image_pixels

    def plot_pixels(self):
        plt.imshow(self.image, cmap='gray')
        plt.show()




