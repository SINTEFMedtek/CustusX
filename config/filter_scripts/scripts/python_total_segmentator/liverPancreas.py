
import os
import SimpleITK as sitk
import sys
import getopt
import glob

from _process_utils import ManagedProcess, describeFailure


def runTotalSegmentator(filenameInput, fastMode=False):
    venv_path = os.path.dirname(sys.executable)
    if not filenameInput.endswith('.nii.gz'):
        filenameInput_nii_gz = os.path.splitext(filenameInput)[0] + '.nii.gz'
        sitk.WriteImage(sitk.ReadImage(filenameInput), filenameInput_nii_gz)
        filenameInput = filenameInput_nii_gz

    args = [venv_path + '/TotalSegmentator', '-i', filenameInput,
            '-o', venv_path + '/../../segmentations', '--task', 'total',
            '--nr_thr_saving', '1']
    if fastMode:
        args.append('--fast')
    process = ManagedProcess(args)

    current_part = 0
    total_parts = 1
    resampling_count = 0

    for line in process.iter_output():
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
    if process.returncode != 0:
        print(describeFailure(process.returncode), flush=True)
        sys.exit(process.returncode)

def copyOutput(filenameInput):
    venv_path = os.path.dirname(sys.executable)
    data_path = venv_path + '/../../segmentations/'
    filenameInputNoExt = os.path.splitext(filenameInput)[0]
    if os.path.isfile(data_path + 'liver.nii.gz'):
        sitk.WriteImage(sitk.ReadImage(data_path + 'liver.nii.gz'), filenameInputNoExt + '_liverPancreas_Liver.mhd')
    if os.path.isfile(data_path + 'pancreas.nii.gz'):
        sitk.WriteImage(sitk.ReadImage(data_path + 'pancreas.nii.gz'), filenameInputNoExt + '_liverPancreas_Pancreas.mhd')

def deleteAllFilesInSegmentationFolder():
    venv_path = os.path.dirname(sys.executable)
    data_path = venv_path + '/../../segmentations/'
    files = glob.glob(data_path + '*')
    for f in files:
        os.remove(f)


def main(argv):
    os.environ["CUDA_VISIBLE_DEVICES"] = "0,1"
    # Reduces fragmentation-driven CUDA allocation failures on GPUs with tight VRAM.
    os.environ.setdefault("PYTORCH_CUDA_ALLOC_CONF", "expandable_segments:True")

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

    fast_mode = '--fast' in sys.argv

    print("PROGRESS: 10", flush=True)
    deleteAllFilesInSegmentationFolder()
    print("PROGRESS: 20", flush=True)
    runTotalSegmentator(input_image_path, fast_mode)
    print("PROGRESS: 90", flush=True)
    copyOutput(input_image_path)
    print("PROGRESS: 100", flush=True)


if __name__ == "__main__":
    main(sys.argv[1:])
