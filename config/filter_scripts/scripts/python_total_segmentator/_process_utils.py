
# Shared process-management helper for the TotalSegmentator wrapper scripts in
# this directory (liverPancreas.py, liverVessels.py, etc). Each script is
# invoked directly as `python <script>.py`, which puts this directory on
# sys.path, so a plain `from _process_utils import ManagedProcess` works
# without any path setup.

import atexit
import os
import select
import signal
import subprocess
import sys
import time


class ManagedProcess:
    """Launches `args` in its own process group and reads its output.

    TotalSegmentator forks its own multiprocessing worker processes, so
    killing only the top-level process can leave those orphaned - still
    holding the GPU, and (via an inherited stdout fd) able to block a naive
    "for line in process.stdout" read loop forever. start_new_session=True
    puts the whole tree in its own process group so killpg() can target all
    of it at once, and kill() always attempts that, even if the top-level
    process has already exited: the OOM killer (or any crash) can take out
    just that one process while leaving its forked workers alive.
    """

    def __init__(self, args):
        self.process = subprocess.Popen(
            args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, bufsize=1, start_new_session=True
        )
        self._pgid = self.process.pid
        atexit.register(self.kill)
        signal.signal(signal.SIGTERM, lambda sig, frame: sys.exit(0))

    def iter_output(self):
        # Read stdout until the process itself exits, not just until the pipe
        # reaches EOF: if it dies uncleanly (e.g. picked by the OOM killer,
        # but not its forked multiprocessing workers - see kill() above),
        # those orphaned workers can keep the pipe's write end open
        # indefinitely, leaving a plain "for line in process.stdout" loop
        # blocked forever.
        process = self.process
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

    def wait(self):
        return self.process.wait()

    @property
    def returncode(self):
        return self.process.returncode

    def kill(self):
        if self._pgid is None:
            return
        try:
            os.killpg(self._pgid, signal.SIGTERM)
        except ProcessLookupError:
            return
        for _ in range(50):  # wait up to 5s for a graceful exit
            try:
                os.killpg(self._pgid, 0)  # raises once the whole group is gone
            except ProcessLookupError:
                return
            time.sleep(0.1)
        try:
            os.killpg(self._pgid, signal.SIGKILL)
        except ProcessLookupError:
            pass
