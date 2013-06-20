#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
# Description:
#
#################################################             

import subprocess
import optparse
import sys
import os.path
import getpass
import platform
import shutil
    
class Shell (object):
    '''
    Superclass for platform specific shells like:
    -cmd (Windows)
    -bash (Mac & Linux)
    '''
    class EvaluateValue:
        'used as return value from shell command'
        def __init__(self, stdout='', returncode=0):
            self.stdout = stdout
            self.returncode = returncode
        def __nonzero__(self):
            'makes type convertible to bool - evaulate to False when nonzero retcode.'
            return self.returncode != 0
            
    def __init__(self):
        self.DUMMY = False
        self.VERBOSE = False
        self.REDIRECT_OUTPUT = False
        self.TERMINATE_ON_ERROR = True
        
    def setDummyMode(self, value):
        shell.DUMMY = value
        if value == True:
            self._printInfo('Running DUMMY mode: no shell executed.')

    def setRedirectOutput(self, value):
        self.REDIRECT_OUTPUT = value

    def run(self, cmd, ignoreFailure=False):
        '''
        Run a shell script
        '''
        cmd = self._convertToString(cmd)
        self._printCommand(cmd)
        if self.DUMMY is False:
            return self._runReal(cmd, ignoreFailure)

    def evaluate(self, cmd):
        '''
        This function takes shell commands and returns stdout.
        An error means that None is returned.
        '''
        cmd = self._convertToString(cmd)
        self._printCommand(cmd)
        if self.DUMMY is False:
            return self._evaluateReal(cmd)
        else:
            return Shell.EvaluateValue()

    def changeDir(self, path):
        path = path.replace("\\", "/")
        if not os.path.exists(path):
            os.makedirs(path)
        self.CWD = path
        self._printCommand('cd %s' % path)
    
    def removeTree(self, path):
        self._printInfo("Removing folder and contents of %s." % path)
        if self.DUMMY is False:
            if os.path.exists(path):
                shutil.rmtree(path, False)

    def _runReal(self, cmd, ignoreFailure):
        '''
        This function runs shell,
        return true if success
        '''
        if self.REDIRECT_OUTPUT:
            p = self._runAndRedirectOutput(cmd)
        else:
            p = self._runDirectly(cmd)            
        if not ignoreFailure:
            self._checkTerminate(p)
        return p.returncode == 0
        
    def _runAndRedirectOutput(self, cmd):
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=self.CWD)        
        for line in self._readFromProcess(p):
            self._printOutput(line.rstrip())
        return p

    def _readFromProcess(self, process):
        'return an iterable object that reads all stdout from process until completed'
        while True:
            retcode = process.poll()
            for line in process.stdout:
                yield line
            if retcode is not None:
                break
        
    def _runDirectly(self, cmd):
        p = subprocess.Popen(cmd, shell=True, cwd=self.CWD)
        p.communicate("") # wait for process to complete
        return p

    def _checkTerminate(self, process):
        if process.returncode!=0 and self.TERMINATE_ON_ERROR:
            self._printInfo("Terminating: shell command exited with a nonzero return value [%s]" % process.returncode)
            quit()
               
    def _evaluateReal(self, cmd):
        '''
        This function takes shell commands and returns stdout.
        An error means that None is returned.
        '''
        retval = []
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=self.CWD)
        for line in self._readFromProcess(p):
            self._printOutput(line.rstrip())
            retval.append(line) 
        #print "pre..:%s" % p.returncode
        #if p.returncode != 0:
        #    return None
        #print "*****".join(retval)
        #return "".join(retval) 
        return Shell.EvaluateValue(stdout="".join(retval), returncode=p.returncode)

                    
    def _printInfo(self, text):
        print '[shell info] %s' % text
    def _printCommand(self, text):
        print '[shell cmd] %s' % text
    def _printOutput(self, text):
        if self.REDIRECT_OUTPUT:
            print '[shell ###] %s' % text
        else:
            print '%s' % text

    @staticmethod
    def create():
        if(platform.system() == 'Windows'):
            return Cmd()
        else:
            return Bash() 
        
    def _convertToString(self, cmd):
        if isinstance(cmd, list):
            cmd = ' '.join(cmd)
        return cmd

        
# ---------------------------------------------------------
    
class Cmd (Shell):
    '''
    Interface to the Windows command shell (cmd.exe).
    '''
    def __init__(self):
        super(Cmd, self).__init__()
        self.CWD = "C:\\"
    
    def ___changeDir(self, path):
        path = path.replace("/", "\\")
        Shell.run(self, 'cmd /C mkdir '+path)
        Shell.changeDir(self, path)
# ---------------------------------------------------------

class Bash (Shell):
    '''
    Wrapper for methods that call the bash shell.
    Also keeps track of the current directory through changeDir()
    '''
    def __init__(self):
        super(Bash, self).__init__()
        self.CWD = '/' # remember directory
    
    def ___changeDir(self, path):
        '''mkdir + cd bash operations'''
        path = path.replace("\\", "/")
        Shell.run(self, 'mkdir -p '+path)
        Shell.changeDir(self, path)
# --------------------------------------------------------

# instantiate shell:
shell = Shell.create()
# shortcuts
runShell = shell.run
changeDir = shell.changeDir

