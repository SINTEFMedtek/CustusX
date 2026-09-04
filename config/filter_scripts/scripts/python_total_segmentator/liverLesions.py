
import os
import SimpleITK as sitk
import sys
import getopt
import glob

from _process_utils import ManagedProcess


def runTotalSegmentator(filenameInput):
    venv_path = os.path.dirname(sys.executable)
    if not filenameInput.endswith('.nii.gz'):
        filenameInput_nii_gz = os.path.splitext(filenameInput)[0] + '.nii.gz'
        sitk.WriteImage(sitk.ReadImage(filenameInput), filenameInput_nii_gz)
        filenameInput = filenameInput_nii_gz

    process = ManagedProcess(
        [venv_path + '/TotalSegmentator', '-i', filenameInput,
         '-o', venv_path + '/../../segmentations', '--task', 'liver_lesions',
         '--nr_thr_saving', '1']
    )

    # liver_lesions has two prediction phases:
    #   1. Fast rough pass (1/1 iters) used for cropping
    #   2. Main pass (many iters) — this is where we show real progress
    predicting_count = 0

    for line in process.iter_output():
        line = line.rstrip()
        print(line, flush=True)
        if 'Predicting' in line:
            predicting_count += 1
        elif 'Resampling' in line:
            if predicting_count == 0:
                print("PROGRESS: 22", flush=True)   # before rough pass
            elif predicting_count == 1:
                print("PROGRESS: 26", flush=True)   # between rough and main pass
        elif 'Saving segmentations' in line:
            print("PROGRESS: 89", flush=True)
        elif '%|' in line and predicting_count >= 2:
            try:
                within_pct = int(line.split('%')[0].strip())
                overall = int(28 + 60 * within_pct / 100)  # 28–88
                print("PROGRESS: {}".format(overall), flush=True)
            except (IndexError, ValueError):
                pass

    process.wait()
    if process.returncode != 0:
        print("ERROR: TotalSegmentator failed with exit code {}".format(process.returncode), flush=True)
        sys.exit(process.returncode)

def copyOutput(filenameInput):
    venv_path = os.path.dirname(sys.executable)
    data_path = venv_path + '/../../segmentations/'
    filenameInputNoExt = os.path.splitext(filenameInput)[0]
    if os.path.isfile(data_path + 'liver_lesions.nii.gz'):
        sitk.WriteImage(sitk.ReadImage(data_path + 'liver_lesions.nii.gz'), filenameInputNoExt + '_liverLesions_LiverLesions.mhd')

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

    print("PROGRESS: 10", flush=True)
    deleteAllFilesInSegmentationFolder()
    print("PROGRESS: 20", flush=True)
    runTotalSegmentator(input_image_path)
    print("PROGRESS: 90", flush=True)
    copyOutput(input_image_path)
    print("PROGRESS: 100", flush=True)


if __name__ == "__main__":
    main(sys.argv[1:])
