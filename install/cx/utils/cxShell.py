#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
# Description:
#
#################################################             

from __future__ import print_function
from __future__ import absolute_import
from builtins import object
import subprocess
import optparse
import sys
import os.path
import getpass
import platform
import shutil
import re
import glob
import argparse        
from . import cxArgParse
from . import cxShellCommand        

      

class Shell (object):
    '''
    Superclass for platform specific shells like:
    -cmd (Windows)
    -bash (Mac & Linux)
    '''
            
    def __init__(self):
        self.DUMMY = False
        self.VERBOSE = False
        self.REDIRECT_OUTPUT = False
        self.TERMINATE_ON_ERROR = True
        self.SILENT = False

    def getArgParser(self):
        p = cxArgParse.ArgumentParser(add_help=False)
        p.add_argument('-d', '--dummy', action='store_true', 
                       dest='DUMMY',
                       help='execute script without calling any shell commands')

        p.add_boolean_inverter('--redirect_output', default=self.REDIRECT_OUTPUT, dest='REDIRECT_OUTPUT',
                               help='Redirect stout/stderr through python. Not doing this can cause stdout mangling on the Jenkins server.')

        return p

    def applyCommandLine(self, arguments):
        'read command line and apply the own argparser to self'
        arguments = self.getArgParser().parse_known_args(args=arguments, namespace=self)[1]
        print('CommandLine: ', vars(self))
        return arguments
    
    def setSilent(self, value):
        self.SILENT = value
            
    def setDummyMode(self, value):
        shell.DUMMY = value
        if value == True:
            self._printInfo('Running DUMMY mode: no shell executed.')

    def setRedirectOutput(self, value):
        self.REDIRECT_OUTPUT = value

    def run(self, cmd, ignoreFailure=False, convertToString=True, keep_output=False, silent=False):
        '''
        Run a shell script, return success/failure in a ShellCommand.ReturnValue object.
        If keep_output is true, include full output from the command as well.
        If silent is true, don't print output to commandline.
        '''
        if(convertToString):
            cmd = self._convertToString(cmd)
        if(not silent):
            self._printCommand(cmd)
        if self.DUMMY is True:
            return cxShellCommand.ShellCommandDummy().run()
                        
        command = cxShellCommand.ShellCommandReal(cmd,
                        cwd=self.CWD, 
                        terminate_on_error=self.TERMINATE_ON_ERROR and not ignoreFailure,
                        redirect_output=self.REDIRECT_OUTPUT,
                        keep_output=keep_output,
                        silent=silent)
        return command.run()

    def evaluate(self, cmd, convertToString=True):
        '''
        This function executes shell commands and returns ShellCommand.ReturnValue
        object describing the results, containing full text output and return code.
        '''
        return self.run(cmd, ignoreFailure=True, convertToString=convertToString, keep_output=True)

    def makeDirs(self, path):
        path = path.replace("\\", "/")
        if not os.path.exists(path):
            os.makedirs(path)

    def changeDir(self, path):
        path = path.replace("\\", "/")
        self.makeDirs(path)
        self.CWD = path
        self._printCommand('cd %s' % path)
    
    def head(self, file, n):
        '''
        Function that mimics the unix command head -nX.
        '''
        open_file = open(file, 'r')
        lines = open_file.readlines()
        line = lines[n-1].rstrip()
        #print 'Line: %s' % line
        return line
    
    def cp(self, src, dst):
        '''
        Function that mimics the unix command cp src dst.
        '''
        destpath = os.path.dirname(dst)
        self.makeDirs(destpath)
        shutil.copy(src, dst)

    def rm_r(self, path, pattern=""):
        path = self._convertToString(path)
        # extract filename component from path is possible.
        if len(pattern)==0 and not os.path.isdir(path):
            pattern = os.path.basename(path)
            path = os.path.dirname(path)            
        # run rm_r recursively on all files in pathS
        self._rm_r_recursive(path, pattern)

    def _rm_r_recursive(self, path, pattern=""):
        '''
        This function mimics rm -rf (unix) for
        Linux, Mac and Windows. Will work with
        Unix style pathname pattern expansion. Not regex.
        '''        
        info = 'Running rm_f on %s' % path
        if len(pattern)!=0:
            info = info + ', pattern=%s' % pattern
        self._printInfo(info)

        if os.path.isdir(path):
            dir = path
            if(pattern == ""):
                shutil.rmtree(dir)
            else:
                matching_files = glob.glob("%s/%s" % (path, pattern))
                for f in matching_files:
                    self._rm_r_recursive(f)
        elif os.path.exists(path):
            os.remove(path)
    
    def removeTree(self, path):
        self._printInfo("Removing folder and contents of %s." % path)
        if self.DUMMY is False:
            if os.path.exists(path):
                shutil.rmtree(path, False)

    def _printInfo(self, text):
        if(not self.SILENT):
            print('[shell info] %s' % text)
            
    def _printCommand(self, text):
        if(not self.SILENT):
                print('[shell cmd] %s' % text)

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

