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
import os
import time
import webbrowser


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


def packageTextAsHTML(heading, body, filename):
	"""
	Create a minimal html file containgin header and body
	"""
	skeleton = """\
<!DOCTYPE html>
<html>
<body>
<h1>%s</h1>
<p>%s</p>
</body>
</html>
"""
	body = '&nbsp&nbsp&nbsp&nbsp&nbsp'.join(body.split('\t'))
	body = '<br>'.join(body.split('\n'))
	#print body
	text = skeleton % (heading, body)
	f = open(filename, 'w')
	f.write(text)
	f.close()
	return filename

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
		"""
		"""
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
		p.add_option('--publish', '-u',
			     action='store_true',
			     help='publish results to medtek.sintef.no/unittest and medtek.sintef.no/coverage',
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

		path = ''
		if len(arguments)>0:
			path = arguments[0]
		# display help if no path set
		if len(path)==0:
			self.optionParser.print_help()
			return

		full = (not options.initialize) and (not options.run_tests) and (not options.post_test) and (not options.publish)

		print 'Running coverage on folder %s' % path
		changeDir(os.path.abspath(path))
		self.mOutputPath = shell.CWD + '/coverage_info'
		self.mCTestResultsFile = '%s/ctest_results.txt' % self.mOutputPath

		if full or options.initialize:
			print 'Initializing ...'
			self._initializeCoverage()

		if full or options.run_tests:
			self._run_ctest(options.ctest_args)

		if full or options.post_test:
			print 'Generating html ...'
			self._generateCoverage()

		if full or options.publish:
			print 'Posting to medtek.sintef.no ...'
			self._publish_ctest()
			self._publishCoverage()

	def _initializeCoverage(self):
		"""
		Initialize lcov by resetting all existing counters
		and initializing/generating counters for all files.
		"""
		runShell('lcov --zerocounters -directory .')
		runShell('lcov --capture --initial --directory . --output-file cx_coverage_base.gcov')
		runShell('rm -r -f %s' % self.mOutputPath)

	def _run_ctest(self, ctest_args):
		"""
		Run ctest with the input arguments,
		"""
		cmd = 'ctest'
		if ctest_args:
			cmd = cmd + ' ' + ctest_args
		#cmd = cmd + ' > %s/ctest_results.txt' % path
		cmd = '%s --output-log %s' % (cmd, self.mCTestResultsFile)
		print 'Running %s ...' % cmd
		runShell('mkdir -p %s' % self.mOutputPath)
		runShell(cmd)
            
	def _publish_ctest(self):
		"""
		post the ctest output to the unittest folder on medtek.sintef.no
		"""
		with open(self.mCTestResultsFile, 'r') as f:
			read_data = f.read()

		htmlFile = '%s.html' % self.mCTestResultsFile
		packageTextAsHTML('Unit tests run %s' % time.strftime('%Y-%m-%d_%H-%M'), read_data, htmlFile)
		webbrowser.open('file://%s' % htmlFile)
		runShell('scp -r %s medtek.sintef.no:/Volumes/medtek_HD/Library/Server/Web/Data/Sites/Default/unittest/index.html' % htmlFile)	

	def _generateCoverage(self):
		"""
		Given that lcov is initialized and ctest is run,
		Generate html output from the gcov data.
		Return output folder name.
		"""
		runShell('lcov --capture --directory . --output-file cx_coverage_test.gcov')
		runShell('lcov --add-tracefile cx_coverage_base.gcov --add-tracefile cx_coverage_test.gcov --output-file cx_coverage_total.gcov')
		runShell('lcov --remove cx_coverage_total.gcov "/eigen3/Eigen/*" "/opt/*" "/external_code/*" "/Library/*" "/usr/*" "/moc*.cxx" "/CustusX3/build_*" "/Examples/*" --output-file cx_coverage.gcov')
#		runShell('lcov --remove cx_coverage_total.gcov "/eigen3/Eigen/*" "/opt/*" "/external_code/*" "/Library/*" "/usr/*" "/moc*.cxx" "/CustusX3/build_*" "/testing/*" "/Testing/*" "/Examples/*" --output-file cx_coverage.gcov')
		runShell('genhtml cx_coverage.gcov --output-directory %s' % self.mOutputPath)

	def _publishCoverage(self):
		"""
		Publish the input folder containing lcov html files to medtek.sintef.no,
		both to the static location coverage accessible from the wiki,
		and to the CustusX/coverage/<timestamp> folder for history storage.

		Additionally, open in a local web browser
		"""
#		inpath = '%s/%s/' % (shell.CWD, htmlFolder)
		indexFile = '%s/index.html' % self.mOutputPath
		print 'Opening %s in browser...' % indexFile 
		#open.webbrowser('file://%s' % indexFile) # not portable on files
		runShell('gvfs-open file://%s' % indexFile)
		runShell('scp -r %s/* medtek.sintef.no:/Volumes/medtek_HD/Library/Server/Web/Data/Sites/Default/coverage' % self.mOutputPath)
		datedpath = '%s_%s' % (self.mOutputPath, time.strftime('%Y-%m-%d_%H-%M'))
		# this assumes htmlFolder is a folder, not a path!
		#print 'htmlfolder: ', htmlFolder
		#print 'datedFolder: ', datedFolder
		#print 'cwd ', os.getcwd()
		#print 'shell.cwd ', shell.CWD
		os.rename(self.mOutputPath, datedpath)
		runShell('scp -r %s medtek.sintef.no:/Volumes/MedTekDisk/Software/CustusX/coverage' % datedpath)
		os.rename(datedpath, self.mOutputPath)


def main():
    Controller()

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()

