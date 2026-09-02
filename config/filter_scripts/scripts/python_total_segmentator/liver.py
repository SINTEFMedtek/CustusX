
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


def runTotalSegmentator(filenameInput, task, progressStart, progressEnd):
    global _child_process
    venv_path = os.path.dirname(sys.executable)
    if not filenameInput.endswith('.nii.gz'):
        filenameInput_nii_gz = os.path.splitext(filenameInput)[0] + '.nii.gz'
        sitk.WriteImage(sitk.ReadImage(filenameInput), filenameInput_nii_gz)
        filenameInput = filenameInput_nii_gz

    process = subprocess.Popen(
        [venv_path + '/TotalSegmentator', '-i', filenameInput,
         '-o', venv_path + '/../../segmentations', '--task', task,
         '--nr_thr_saving', '1'],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, bufsize=1
    )
    _child_process = process

    # Each task has two prediction phases:
    #   1. Fast rough pass (1/1 iters) used for cropping
    #   2. Main pass (many iters) — this is where we show real progress
    predicting_count = 0
    span = progressEnd - progressStart

    for line in process.stdout:
        line = line.rstrip()
        print(line, flush=True)
        if 'Predicting' in line:
            predicting_count += 1
        elif 'Resampling' in line:
            if predicting_count <= 1:
                print("PROGRESS: {}".format(int(progressStart + span * 0.15)), flush=True)
        elif 'Saving segmentations' in line:
            print("PROGRESS: {}".format(int(progressStart + span * 0.9)), flush=True)
        elif '%|' in line and predicting_count >= 2:
            try:
                within_pct = int(line.split('%')[0].strip())
                overall = int(progressStart + span * (0.2 + 0.65 * within_pct / 100))
                print("PROGRESS: {}".format(overall), flush=True)
            except (IndexError, ValueError):
                pass

    process.wait()
    if process.returncode != 0:
        print("ERROR: TotalSegmentator failed with exit code {}".format(process.returncode), flush=True)
        sys.exit(process.returncode)

def copyOutput(filenameInputNoExt, data_path, sourceName, targetSuffix):
    sourceFile = data_path + sourceName
    if os.path.isfile(sourceFile):
        sitk.WriteImage(sitk.ReadImage(sourceFile), filenameInputNoExt + '_liver_{}.mhd'.format(targetSuffix))

def deleteAllFilesInSegmentationFolder(data_path):
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

    venv_path = os.path.dirname(sys.executable)
    data_path = venv_path + '/../../segmentations/'
    filenameInputNoExt = os.path.splitext(input_image_path)[0]

    print("PROGRESS: 2", flush=True)
    deleteAllFilesInSegmentationFolder(data_path)

    # 'total': body-wide task, used here for the liver/pancreas parenchyma
    runTotalSegmentator(input_image_path, 'total', 2, 28)
    copyOutput(filenameInputNoExt, data_path, 'liver.nii.gz', 'Liver')
    copyOutput(filenameInputNoExt, data_path, 'pancreas.nii.gz', 'Pancreas')
    deleteAllFilesInSegmentationFolder(data_path)

    # 'liver_vessels': also outputs liver_tumor.nii.gz, but that is covered by
    # the dedicated (newer, more accurate) liver_lesions task below instead.
    runTotalSegmentator(input_image_path, 'liver_vessels', 28, 52)
    copyOutput(filenameInputNoExt, data_path, 'liver_vessels.nii.gz', 'LiverVessels')
    deleteAllFilesInSegmentationFolder(data_path)

    runTotalSegmentator(input_image_path, 'liver_lesions', 52, 74)
    copyOutput(filenameInputNoExt, data_path, 'liver_lesions.nii.gz', 'LiverLesions')
    deleteAllFilesInSegmentationFolder(data_path)

    # 'liver_segments': Couinaud segments 1-8
    runTotalSegmentator(input_image_path, 'liver_segments', 74, 98)
    for segment in range(1, 9):
        copyOutput(filenameInputNoExt, data_path, 'liver_segment_{}.nii.gz'.format(segment), 'LiverSegment{}'.format(segment))

    print("PROGRESS: 100", flush=True)


if __name__ == "__main__":
    main(sys.argv[1:])
