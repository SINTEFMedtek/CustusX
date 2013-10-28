#!/usr/bin/env python

#####################################################
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.10.16
#
#################################################  

import subprocess

class ShellCommand:
    class ReturnValue:
        'used as return value from shell command'
        def __init__(self, stdout=None, returncode=0, process=None):
            self.stdout = stdout
            self.returncode = returncode
            self.process = process
        def __nonzero__(self):
            'makes type convertible to bool - evaluate to True when zero retcode.'
            return self.returncode == 0
    
    def run(self):
        pass
    
class ShellCommandDummy(ShellCommand):
    def run(self):
        return ShellCommand.ReturnValue()

class ShellCommandReal(ShellCommand):
    def __init__(self, command, 
                 cwd,
                 terminate_on_error=True, 
                 redirect_output=False, 
                 keep_output=False,
                 TEST_add_win_env=False):
        self.command = command
        self.cwd = cwd
        
        self.redirect_output = redirect_output
        self.keep_output = keep_output
        self.terminate_on_error = terminate_on_error
        self.TEST_add_win_env = TEST_add_win_env
    
    def run(self):
        '''
        Run the shell command, return a ReturnValue object:
         - evaluates to false if call failed.
         - contains output and returncode for more detailed results
        '''
        if self.TEST_add_win_env == True:
            self.command = "C:\\Dev\\CustusX_environment\\cxVars_x64.bat && " + self.command
        
        if  self.redirect_output or self.keep_output:
            p = self._runAndRedirectOutput(self.command)
        else:        
            p = self._runDirectly(self.command)            
        self._checkTerminate(p)
        return p        
        
    def _runDirectly(self, cmd):
        p = subprocess.Popen(cmd, shell=True, cwd=self.cwd)
        p.communicate("") # wait for process to complete
        return ShellCommand.ReturnValue(returncode=p.returncode, process=p)

    def _runAndRedirectOutput(self, cmd):
        output = []
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=self.cwd)        
        for line in self._readFromProcess(p):
            self._printOutput(line.rstrip())
            if self.keep_output:
                output.append(line) 
        return ShellCommand.ReturnValue(stdout="".join(output), returncode=p.returncode, process=p)
#        return p

    def _readFromProcess(self, process):
        'return an iterable object that reads all stdout from process until completed'
        while True:
            retcode = process.poll()
            for line in process.stdout:
                yield line
            if retcode is not None:
                break
        
    def _checkTerminate(self, retval):
        if retval.returncode!=0 and self.terminate_on_error:
            test = "Terminating: shell command exited with a nonzero return value [%s]" % retval.returncode
            self._printInfo(test)
            exit(test)
            
    def _printInfo(self, text):
        print '[shell info] %s' % text
    def _printOutput(self, text):
        if self.redirect_output:
            print '[shell ###] %s' % text
        else:
            print '%s' % text
            
