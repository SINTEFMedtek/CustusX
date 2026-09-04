
import atexit
import os
import select
import signal
import subprocess
import SimpleITK as sitk
import sys
import getopt
import glob
import time

_child_process = None
_child_pgid = None

def _kill_child():
    # TotalSegmentator forks its own multiprocessing worker processes, so
    # killing only the top-level process can leave those orphaned (still
    # holding GPU memory) - start_new_session=True below puts the whole tree
    # in its own process group, and killpg targets all of it at once. This
    # always attempts to kill the group, even if the top-level process has
    # already exited: the OOM killer (or any crash) can take out just that
    # one process while leaving its forked workers alive, still holding the
    # GPU and (via an inherited stdout fd) able to block our own read loop.
    global _child_process, _child_pgid
    if _child_pgid is None:
        return
    try:
        os.killpg(_child_pgid, signal.SIGTERM)
    except ProcessLookupError:
        return
    for _ in range(50):  # wait up to 5s for a graceful exit
        try:
            os.killpg(_child_pgid, 0)  # raises once the whole group is gone
        except ProcessLookupError:
            return
        time.sleep(0.1)
    try:
        os.killpg(_child_pgid, signal.SIGKILL)
    except ProcessLookupError:
        pass

atexit.register(_kill_child)
signal.signal(signal.SIGTERM, lambda sig, frame: sys.exit(0))


def _iter_output(process):
    # Read stdout until the process itself exits, not just until the pipe
    # reaches EOF: if it dies uncleanly (e.g. picked by the OOM killer, but
    # not its forked multiprocessing workers - see _kill_child() above),
    # those orphaned workers can keep the pipe's write end open indefinitely,
    # leaving a plain "for line in process.stdout" loop blocked forever.
    while True:
        ready, _, _ = select.select([process.stdout], [], [], 0.5)
        if ready:
            line = process.stdout.readline()
            if line == '':
                return
            yield line
            continue
        if process.poll() is not None:
            while True:
                ready2, _, _ = select.select([process.stdout], [], [], 0)
                if not ready2:
                    return
                line = process.stdout.readline()
                if line == '':
                    return
                yield line


def runTotalSegmentator(filenameInput):
    global _child_process, _child_pgid
    venv_path = os.path.dirname(sys.executable)
    if not filenameInput.endswith('.nii.gz'):
        filenameInput_nii_gz = os.path.splitext(filenameInput)[0] + '.nii.gz'
        sitk.WriteImage(sitk.ReadImage(filenameInput), filenameInput_nii_gz)
        filenameInput = filenameInput_nii_gz

    process = subprocess.Popen(
        [venv_path + '/TotalSegmentator', '-i', filenameInput,
         '-o', venv_path + '/../../segmentations', '--task', 'liver_lesions',
         '--nr_thr_saving', '1'],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, bufsize=1, start_new_session=True
    )
    _child_process = process
    _child_pgid = process.pid

    # liver_lesions has two prediction phases:
    #   1. Fast rough pass (1/1 iters) used for cropping
    #   2. Main pass (many iters) — this is where we show real progress
    predicting_count = 0

    for line in _iter_output(process):
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
