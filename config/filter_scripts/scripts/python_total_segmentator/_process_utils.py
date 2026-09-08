
# Shared process-management helper for the TotalSegmentator wrapper scripts in
# this directory (liverPancreas.py, liverVessels.py, etc). Each script is
# invoked directly as `python <script>.py`, which puts this directory on
# sys.path, so a plain `from _process_utils import ManagedProcess` works
# without any path setup.

import atexit
import os
import select
import shutil
import signal
import subprocess
import sys
import time


def describeFailure(returncode):
    if returncode == -9:
        return ("ERROR: TotalSegmentator was killed (signal 9) - most likely because it exceeded "
                 "the memory limit (see _wrapWithMemoryLimit() below), or a stop was requested. "
                 "Try enabling Fast mode, raising the memory limit in the segmentation widget's "
                 "advanced options, or closing other applications.")
    return "ERROR: TotalSegmentator failed with exit code {}".format(returncode)


def _memoryCapBytes():
    # TotalSegmentator's own memory use has repeatedly been observed
    # ballooning past 20-25GB and taking the whole machine down via
    # swap-thrashing (confirmed via kernel OOM-killer logs) rather than
    # being cleanly killed - leave enough headroom for the rest of the
    # system (CustusX/CustusS itself, the desktop, other applications)
    # by capping at a fraction of total physical RAM, not a fixed value.
    # CX_TOTALSEGMENTATOR_MEMORY_LIMIT_GB (set from the GUI's advanced
    # options) overrides this automatic default when present.
    override = os.environ.get('CX_TOTALSEGMENTATOR_MEMORY_LIMIT_GB')
    if override:
        try:
            return int(float(override) * 1024 ** 3)
        except ValueError:
            pass
    try:
        total = os.sysconf('SC_PHYS_PAGES') * os.sysconf('SC_PAGE_SIZE')
    except (ValueError, OSError, AttributeError):
        return None
    return int(total * 0.6)


def _wrapWithMemoryLimit(args):
    # Runs args in a transient systemd scope with a memory cgroup limit and
    # no swap allowed, so hitting the cap kills just this process tree
    # cleanly (surfacing as a normal non-zero exit code, same as any other
    # TotalSegmentator failure) instead of the whole system swap-thrashing
    # into unresponsiveness. Falls back to an unrestricted launch if
    # systemd-run isn't available - this is defense in depth, not the
    # primary way of keeping memory use in check.
    systemdRun = shutil.which('systemd-run')
    cap = _memoryCapBytes()
    if not systemdRun or not cap:
        return args
    return [systemdRun, '--user', '--scope', '--quiet', '--collect',
            '-p', 'MemoryMax={}'.format(cap),
            '-p', 'MemorySwapMax=0',
            '--'] + args


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
            _wrapWithMemoryLimit(args), stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
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
