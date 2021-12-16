#!/usr/bin/env python

#####################################################
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.10.16
#
#################################################  

from __future__ import print_function
from builtins import object
import subprocess
import platform


class ShellCommand(object):
    class ReturnValue(object):
        'used as return value from shell command'

        def __init__(self, stdout=None, returncode=0, process=None):
            self.stdout = stdout
            self.returncode = returncode
            self.process = process

        def __bool__(self):
            'makes type convertible to bool - evaluate to True when zero retcode.'
            return self.returncode == 0

    def run(self):
        pass


class ShellCommandDummy(ShellCommand):
    def run(self):
        return ShellCommand.ReturnValue()


class ShellCommandReal(ShellCommand):
    '''
    If silent is true, don't print output to commandline.
    '''

    def __init__(self, command,
                 cwd,
                 terminate_on_error=True,
                 redirect_output=False,
                 keep_output=False,
                 silent=False):
        self.command = command
        self.cwd = cwd
        self.redirect_output = redirect_output
        self.keep_output = keep_output
        self.terminate_on_error = terminate_on_error
        self.silent = silent

    def run(self):
        '''
        Run the shell command, return a ReturnValue object:
         - evaluates to false if call failed.
         - contains output and returncode for more detailed results
        '''
        if self.redirect_output or self.keep_output:
            p = self._runAndRedirectOutput(self.command)
        else:
            p = self._runDirectly(self.command)
        self._checkTerminate(p)
        return p

    def _get_popen_executable(self):
        if platform.system() != 'Windows':
            return '/bin/bash'
        else:
            return None

    def _runDirectly(self, cmd):
        p = subprocess.Popen(cmd, shell=True, cwd=self.cwd, executable=self._get_popen_executable())
        p.communicate("")  # wait for process to complete
        p.returncode = self._convertCatchReturnCode139ToSegfault(p.returncode)
        return ShellCommand.ReturnValue(returncode=p.returncode, process=p)

    def _convertCatchReturnCode139ToSegfault(self, returncode):
        '''
        TODO: Fix this in a better way
        Retcode 139 means faulty memory modules, but may sometimes be returned instead of -11 (segfaults) on Ubuntu
        https://wiki.ubuntu.com/DebuggingInstallationIssues#Segmentation_Fault_-_Exit_status_139
        http://ubuntuforums.org/archive/index.php/t-2128220.html
        We may have to run memory tests first to determine if the memory are faulty,
        so the temporary fix below will hide this info
        Memory test on AMD lab rack found no errors
        '''
        if (platform.system() == 'Linux'):
            if returncode == 139:
                returncode = -11
        return returncode

    def _runAndRedirectOutput(self, cmd):
        output = []
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=self.cwd, executable=self._get_popen_executable())
        for line in self._readFromProcess(p):
            if(not self.silent):
                self._printOutput(line.rstrip())
            if self.keep_output:
                if type(line) is bytes:
                    line = line.decode()
                output.append(line)
        p.returncode = self._convertCatchReturnCode139ToSegfault(p.returncode)
        return ShellCommand.ReturnValue(stdout="".join(output), returncode=p.returncode, process=p)

    def _readFromProcess(self, process):
        'return an iterable object that reads all stdout from process until completed'
        while True:
            retcode = process.poll()
            for line in process.stdout:
                yield line
            if retcode is not None:
                break

    def _checkTerminate(self, retval):
        if retval.returncode != 0 and self.terminate_on_error:
            test = "Terminating: shell command exited with a nonzero return value [%s]" % retval.returncode
            self._printInfo(test)
            exit(test)

    def _printInfo(self, text):
        print('[shell info] %s' % text)

    def _printOutput(self, text):
        if self.redirect_output:
            print('[shell ###] %s' % text)
        else:
            print('%s' % text)
