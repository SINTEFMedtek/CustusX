#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.01.30
#
# Description:
#
#             
#################################################             

import subprocess
import optparse
import re
import sys
import os.path
import urllib
import getpass
import platform

class Common(object):
    '''
    Container for common data.
    Modify these to change behaviour
    '''
    def __init__(self):
	pass
# ---------------------------------------------------------
    
class Shell (object):
    '''
    Superclass for platform specific shells like:
    -cmd (Windows)
    -bash (Mac & Linux)
    '''
    def __init__(self):
        self.DUMMY = False
        self.VERBOSE = False
        self.CWD = '/' # remember directory
        self.password = ""
        
    def _runReal(self, cmd):
        '''This function runs shell, no return, insert password'''
        print '*** run:', cmd
        p = subprocess.Popen(cmd, shell=True, cwd=self.CWD)
        p.communicate("") # seems to be needed in order to wait for password prompting...?
    
    def _runDummy(self, cmd):
        '''Dummy version of shell'''
        if cmd=="whoami":
            #return self._runReal(cmd) #does not work as expected on windows
            return DATA.mUser
        print "*** dummy run:", cmd
        return ""
    
    def run(self, cmd):
        '''Run a shell script'''
        if self.DUMMY is True:
            return self._runDummy(cmd)
        else:
            return self._runReal(cmd)
           
    def evaluate(self, cmd):
        '''This function takes shell commands and returns them'''
        # print '*** eval run:', cmd
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=self.CWD)
        out = p.stdout.read().strip()
        return out #This is the stdout from the shell command
    
    def changeDir(self, path):
        self.CWD = path
    
# ---------------------------------------------------------
    
class Bash (Shell):
    '''
    Wrapper for methods that call the bash shell.
    Also keeps track of the current directory through changeDir()
    '''
    def __init__(self):
        super(Bash, self).__init__()
    
    def changeDir(self, path):
        '''mkdir + cd bash operations'''
        path = path.replace("\\", "/")
        #Shell.run(self, 'mkdir -p '+path)
        Shell.changeDir(self, path)

# --------------------------------------------------------

shell = Bash() 
# shortcuts
runShell = shell.run
changeDir = shell.changeDir

# ---------------------------------------------------------

class Controller(object):
    '''
    A command line program that parses options and arguments,
    then performs the requested operations on the selected
    components.
    '''
    def __init__(self):
    	'''
    	Initialize and run the controller
    	'''        
        self.optionParser = self._createOptionParser();
        self._parseCommandLine()
    
    def _createOptionParser(self):
        description='''
Coverage script for CustusX and its components.
Uses lcov to run ctest and generate coverage reports from them.
%s.
'''        
        #Create instance of OptionParser Module, included in Standard Library
        p = optparse.OptionParser(description=description,
                                    version='%prog version 0.1',
                                    usage= '%prog [options] [components]')
        p.add_option('--initialize', '-i',
                     action='store_true',
                     help='reset all counters and generate zero counters for all lines',
                     default=False)
        p.add_option('--post_test', '-p',
                     action='store_true',
                     help='generate test html data from the tests',
                     default=False)
        p.add_option('--full', '-f',
                     action='store_true',
                     help='initialize, run tests, generate html data',
                     default=False)
        p.add_option('--run_tests', '-t',
                     action='store_true',
                     help='run ctest',
                     default=False)
        p.add_option('--ctest_args', '-c',
                     action='store',
                     type='string',
                     help='arguments to pass to ctest',
                     default="")
        return p
    
    def _parseCommandLine(self):
        '''
        Parse the options and arguments from the command line
        '''
        options, arguments = self.optionParser.parse_args()

                  
        path = ""
	if len(arguments)>0:
		path = arguments[0]
        # display help if no path set
        if len(path)==0:
        	self.optionParser.print_help()
		return

	full = (not options.initialize) and (not options.run_tests) and (not options.post_test)

	print "Running coverage on folder %s" % path
	changeDir(path)

	if full or options.initialize:
		print "Initializing ..."
		runShell("""\
lcov \
--zerocounters \
-directory . \
""")
		runShell("""\
lcov \
--capture \
--initial \
--directory . \
--output-file cx_coverage_base.gcov \
""")

	if full or options.run_tests:
		cmd = "ctest"
		if options.ctest_args:
			cmd = cmd + " " + options.ctest_args
		print "Running %s ..." % cmd
		runShell(cmd)
	if full or options.post_test:
		print "Generating html ..."
		runShell("""\
lcov \
--capture \
--directory . \
--output-file cx_coverage_test.gcov \
""")
		runShell("""\
lcov \
-add-tracefile cx_coverage_base.gcov \
-add-tracefile cx_coverage_test.gcov \
-o cx_coverage_total.gcov \
""")
		runShell("""\
lcov \
--remove cx_coverage_total.gcov \
'eigen3/Eigen/*' \
'/opt/*' \
'external_code/*' \
'/Library/*' \
'/usr/*' \
'/moc*.cxx' \
'/CustusX3_build_*' \
'/testing/' \
'/Testing/' \
'/Examples/' \
--output-file cx_coverage.gcov \
""")
		runShell("""\
genhtml \
cx_coverage.gcov \
-output-directory ./coverage_info \
""")
		runShell("xdg-open ./coverage_info/index.html")

def main():
    Controller()

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()

