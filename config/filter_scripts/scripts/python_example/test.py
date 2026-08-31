import sys, os, shutil


print("Example script is running. ")
print("Python version: ", sys.version)
print('Number of arguments:', len(sys.argv), 'arguments.')
print('Argument List:', str(sys.argv))

# Copied from custus_utilities to make tests run
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

# Duplcate input volume:
input_image_path = sys.argv[1] # First argument should always be input volume
output_image_path = sys.argv[2] # Second argument should always be destination volume file
input_volume = custusVolume(input_image_path)
input_volume.duplicate(output_image_path)
