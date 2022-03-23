# Automatic liver segmentation in CT using deep learning

#### NOTE: Trained 2D model on the LITS dataset is automatically downloaded when running the inference script and can be used as you wish, but please, give credit. ENJOY! :)


![Screenshot](figures/Segmentation_CustusX.PNG)

The figure shows a predicted liver with the corresponding patient CT in 3DSlicer. It is the Volume-10 from the LITS17 dataset.

First of all:
The LITS dataset can be accessible from here (https://competitions.codalab.org), and the corresponding paper for the challenge (Bilic. P et al.. (2019). The Liver Tumor Segmentation Benchmark (LiTS). https://arxiv.org/abs/1901.04056). If trained model is used please cite this paper.

Usage:
> git clone https://github.com/andreped/livermask.git \
> cd livermask \
> python3 -m venv venv \
> python -m pip install -r /path/to/requirements.txt . \   <- might want to run > python setup.py bdist_wheel < before
> cd livermask \
> python livermask.py "path_to_ct_nifti.nii" "output_name.nii" 

If you lack any modules after, try installing them through setup.py (could be done instead of using requirements.txt):
> pip install wheel \
> python setup.py bdist_wheel

NOTE: Currently, model only works for the nifti format, and outputs a binary volume in the same format (*.nii). But this format can be imported in CustusX. I wouldn't recommend mixing DICOM and .nii prediction file in CustusX, as there seem to be some orientation issues between these (bug to be fixed in the future). But simply convert DICOM -> NIFTI using the command-line tool dcm2niix (https://github.com/rordenlab/dcm2niix).

Convert DICOM -> NIFTI doing this:
> dcm2niix -s y -m y -d 1 "path_to_CT_folder" "output_name"

Note that "-d 1" assumed that "path_to_CT_folder" is the folder just before the set of DICOM scans you want to import and convert. This can be removed if you want to convert multiple ones at the same time. It is possible to set "." for "output_name", which in theory should output a file with the same name as the DICOM folder, but that doesn't seem to happen...

A few final notes:
1) If you get SSLError during downloading the model, disable VPN, e.g. cisco. For those on the sintef network, try changing network to Eduroam or similar, as it might be a most-famous evry-issue...
