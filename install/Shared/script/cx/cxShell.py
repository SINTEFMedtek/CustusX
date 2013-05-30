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
    def __init__(self):
        self.DUMMY = False
        self.VERBOSE = False
        self.TERMINATE_ON_ERROR = True
        
    def setDummyMode(self, value):
        shell.DUMMY = value
        if value == True:
            print '*** Running DUMMY mode: no shell executed.'

    def _runReal(self, cmd, ignoreFailure):
        '''
        This function runs shell,
        return true if success
        '''
        print '*** run:', cmd
        
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=self.CWD)
        while(True):
            retcode = p.poll() #returns None while subprocess is running
            #print "IN WHILE, ", retcode
            while True:
                line = p.stdout.readline()
                if line=='':
                    break
                print "### ", line.rstrip()
            if retcode is not None: 
                break # process terminated
        
        #p = subprocess.Popen(cmd, shell=True, cwd=self.CWD)
        p.wait()
        #p.communicate("") # seems to be needed in order to wait for password prompting...?
        if not ignoreFailure:
            self._checkTerminate(p)
        return p.returncode == 0
    
    def _runDummy(self, cmd):
        '''Dummy version of shell'''
        print "*** dummy run:", cmd
        return ""

    def _checkTerminate(self, process):
        if process.returncode!=0 and self.TERMINATE_ON_ERROR:
            print "*** Terminating: shell command exited with a nonzero return value [%s]" % process.returncode
            quit()
    
    def run(self, cmd, ignoreFailure=False):
        '''Run a shell script'''
        cmd = self._convertToString(cmd)
        if self.DUMMY is True:
            return self._runDummy(cmd)
        else:
            return self._runReal(cmd, ignoreFailure)

    def evaluate(self, cmd):
        '''Run a shell script'''
        cmd = self._convertToString(cmd)
        if self.DUMMY is True:
            return self._runDummy(cmd)
        else:
            return self._evaluateReal(cmd)
           
    def _evaluateReal(self, cmd):
        '''
        This function takes shell commands and returns stdout.
        An error means that None is returned.
        '''
        print '*** eval run:', cmd
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=self.CWD)
        p.wait()
        out = p.stdout.read() #This is the stdout from the shell command
        print out
        #print "retcode:", p.returncode
        if p.returncode != 0:
            return None
        return out.strip() 
    
    def changeDir(self, path):
        path = path.replace("\\", "/")
        if not os.path.exists(path):
            os.makedirs(path)
        self.CWD = path
        print '*** cd %s' % path
    
    def removeTree(self, path):
        print "*** Removing folder and contents of %s." % path
        if self.DUMMY is False:
            if os.path.exists(path):
                shutil.rmtree(path, False)

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

