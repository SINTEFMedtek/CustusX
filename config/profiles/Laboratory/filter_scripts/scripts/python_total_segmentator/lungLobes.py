
import atexit
import os
import signal
import subprocess
import SimpleITK as sitk
import sys
import getopt
import glob

_child_process = None

def _kill_child():
    global _child_process
    if _child_process is not None and _child_process.poll() is None:
        _child_process.kill()

atexit.register(_kill_child)
signal.signal(signal.SIGTERM, lambda sig, frame: sys.exit(0))


def runTotalSegmentator(filenameInput):
    global _child_process
    venv_path = os.path.dirname(sys.executable)
    if os.path.splitext(filenameInput)[1:] != '.nii.gz':
        filenameInput_nii_gz = os.path.splitext(filenameInput)[0] + '.nii.gz'
        sitk.WriteImage(sitk.ReadImage(filenameInput), filenameInput_nii_gz)
        filenameInput = filenameInput_nii_gz

    process = subprocess.Popen(
        [venv_path + '/TotalSegmentator', '-i', filenameInput,
         '-o', venv_path + '/../../segmentations', '--task', 'total'],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, bufsize=1
    )
    _child_process = process

    current_part = 0
    total_parts = 1
    resampling_count = 0

    for line in process.stdout:
        line = line.rstrip()
        print(line, flush=True)
        if 'Predicting part' in line:
            try:
                tokens = line.split()
                current_part = int(tokens[2])
                total_parts = int(tokens[4])
            except (IndexError, ValueError):
                pass
        elif 'Resampling' in line:
            resampling_count += 1
            print("PROGRESS: {}".format(22 if resampling_count == 1 else 88), flush=True)
        elif 'Saving segmentations' in line:
            print("PROGRESS: 89", flush=True)
        elif '%|' in line and current_part > 0:
            try:
                within_pct = int(line.split('%')[0].strip())
                part_done = (current_part - 1) + within_pct / 100.0
                overall = int(23 + 65 * part_done / total_parts)
                print("PROGRESS: {}".format(overall), flush=True)
            except (IndexError, ValueError):
                pass

    process.wait()

def copyOutput(filenameInput):
    venv_path = os.path.dirname(sys.executable)
    data_path = venv_path + '/../../segmentations/'
    filenameInputNoExt = os.path.splitext(filenameInput)[0]
    if os.path.isfile(data_path + 'lung_lower_lobe_left.nii.gz'):
        sitk.WriteImage(sitk.ReadImage(data_path + 'lung_lower_lobe_left.nii.gz'), filenameInputNoExt + '_lobe_LobeLLL.mhd')
    if os.path.isfile(data_path + 'lung_upper_lobe_left.nii.gz'):
        sitk.WriteImage(sitk.ReadImage(data_path + 'lung_upper_lobe_left.nii.gz'), filenameInputNoExt + '_lobe_LobeLUL.mhd')
    if os.path.isfile(data_path + 'lung_lower_lobe_right.nii.gz'):
        sitk.WriteImage(sitk.ReadImage(data_path + 'lung_lower_lobe_right.nii.gz'), filenameInputNoExt + '_lobe_LobeRLL.mhd')
    if os.path.isfile(data_path + 'lung_middle_lobe_right.nii.gz'):
        sitk.WriteImage(sitk.ReadImage(data_path + 'lung_middle_lobe_right.nii.gz'), filenameInputNoExt + '_lobe_LobeRML.mhd')
    if os.path.isfile(data_path + 'lung_upper_lobe_right.nii.gz'):
        sitk.WriteImage(sitk.ReadImage(data_path + 'lung_upper_lobe_right.nii.gz'), filenameInputNoExt + '_lobe_LobeRUL.mhd')

def deleteAllFilesInSegmentationFolder():
    venv_path = os.path.dirname(sys.executable)
    data_path = venv_path + '/../../segmentations/'
    files = glob.glob(data_path + '*')
    for f in files:
        os.remove(f)


def main(argv):
    os.environ["CUDA_VISIBLE_DEVICES"] = "0,1"

    try:
        opts, args = getopt.getopt(argv, "ht:a:", ["Task=", "Arguments="])
    except getopt.GetoptError:
        print('usage: main.py -Task <TaskName> --Arguments <ArgumentsList>')
        sys.exit(2)

    if len(sys.argv) > 1:
        input_image_path = sys.argv[1]
        print('Input file: ' + input_image_path)
    else:
        print('Too few arguments, script aborted.')
        exit(1)

    print("PROGRESS: 10", flush=True)
    deleteAllFilesInSegmentationFolder()
    print("PROGRESS: 20", flush=True)
    runTotalSegmentator(input_image_path)
    print("PROGRESS: 90", flush=True)
    copyOutput(input_image_path)
    print("PROGRESS: 100", flush=True)


if __name__ == "__main__":
    main(sys.argv[1:])
