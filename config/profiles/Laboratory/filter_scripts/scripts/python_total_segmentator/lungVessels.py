
import os
import SimpleITK as sitk
import sys
import getopt
import shutil
import glob


def runTotalSegmentator(filenameInput):
    venv_path = os.path.dirname(sys.executable) 
    if os.path.splitext(filenameInput)[1:] != '.nii.gz':
        filenameInput_nii_gz = os.path.splitext(filenameInput)[0] + '.nii.gz'
        sitk.WriteImage(sitk.ReadImage(filenameInput), filenameInput_nii_gz)
        os.system (venv_path + '/TotalSegmentator -i' + filenameInput_nii_gz + ' -o ' + venv_path + '/../../segmentations --task lung_vessels')
    else:
        os.system(venv_path + '/TotalSegmentator -i' + filenameInput + ' -o ' + venv_path + '/../../segmentations --task lung_vessels')
        
def copyOutput(filenameInput):
    venv_path = os.path.dirname(sys.executable)
    data_path =  venv_path + '/../../segmentations/'
    filenameInputNoExt = os.path.splitext(filenameInput)[0]
    if(os.path.isfile(data_path + 'lung_vessels.nii.gz')):
        sitk.WriteImage(sitk.ReadImage(data_path + 'lung_vessels.nii.gz'), filenameInputNoExt + '_lungVessels.mhd')

def deleteAllFilesInSegmentationFolder():
    venv_path = os.path.dirname(sys.executable)
    data_path =  venv_path + '/../../segmentations/'
    files = glob.glob(data_path + '*')
    for f in files:
        os.remove(f)

InputVolume = ''
def read_arguments(arguments):
    global InputVolume

    arg_list = arguments.strip().split(',')
    #print('arg_list: ' + arg_list)
    print('---')
    print(arg_list)
    print('---')
    pairs = [[x.split(' ')[0], x.split(' ')[1]] for x in arg_list]
    param_dict = dict(pairs)
    InputVolume = param_dict['InputVolume']

    if InputVolume == '':
        print('Input volume is empty, cannot proceed!')
        sys.exit()

    print(InputVolume)


def main(argv):
    os.environ["CUDA_VISIBLE_DEVICES"] = "0,1"

    arguments = ''
    task = ''
    try:
        opts, args = getopt.getopt(argv, "ht:a:", ["Task=", "Arguments="])
    except getopt.GetoptError:
        print('usage: main.py -Task <TaskName> --Arguments <ArgumentsList>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('main.py -Task <TaskName> --Arguments <ArgumentsList>')
            sys.exit()
        elif opt in ("-t", "--Task"):
            task = arg
        elif opt in ("-a", "--Arguments"):
            arguments = arg

    # Constants and input variables
    n_argin_expected = 1  # Expect input volume path
    if len(sys.argv) > n_argin_expected:  # command string is sys.argv[0]
        input_image_path = sys.argv[1]  # First argument should always be input volume
        print('Input file: ' + input_image_path)
    else:
        print('Too few arguments, script aborted.')
        exit(1)  # TODO: Find proper exit code
    # Get input arguments
    print('Arguments: ' + arguments)
    #read_arguments(arguments)


    deleteAllFilesInSegmentationFolder()
    runTotalSegmentator(input_image_path)

    copyOutput(input_image_path)


if __name__ == "__main__":
    main(sys.argv[1:])


