
import atexit
import functools
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
    if not filenameInput.endswith('.nii.gz'):
        filenameInput_nii_gz = os.path.splitext(filenameInput)[0] + '.nii.gz'
        sitk.WriteImage(sitk.ReadImage(filenameInput), filenameInput_nii_gz)
        filenameInput = filenameInput_nii_gz

    process = subprocess.Popen(
        [venv_path + '/TotalSegmentator', '-i', filenameInput,
         '-o', venv_path + '/../../segmentations', '--task', 'liver_segments',
         '--nr_thr_saving', '1'],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, bufsize=1
    )
    _child_process = process

    # liver_segments has two prediction phases:
    #   1. Fast rough pass (1/1 iters) used for cropping
    #   2. Main pass (many iters) — this is where we show real progress
    predicting_count = 0

    for line in process.stdout:
        line = line.rstrip()
        print(line, flush=True)
        if 'Predicting' in line:
            predicting_count += 1
        elif 'Resampling' in line:
            if predicting_count == 0:
                print("PROGRESS: 22", flush=True)   # before rough pass
            elif predicting_count == 1:
                print("PROGRESS: 26", flush=True)   # before main pass
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
    segmentImages = []
    for segment in range(1, 9):
        sourceFile = data_path + 'liver_segment_{}.nii.gz'.format(segment)
        if os.path.isfile(sourceFile):
            image = sitk.ReadImage(sourceFile)
            sitk.WriteImage(image, filenameInputNoExt + '_liverSegments_LiverSegment{}.mhd'.format(segment))
            segmentImages.append(image)

    # TotalSegmentator's liver_segments task has no combined whole-liver output of
    # its own, so derive one as the union of the 8 Couinaud segments.
    if segmentImages:
        combinedImage = functools.reduce(sitk.Maximum, segmentImages)
        sitk.WriteImage(combinedImage, filenameInputNoExt + '_liverSegments_LiverSegmentsCombined.mhd')

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
