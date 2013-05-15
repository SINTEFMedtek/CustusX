#!/usr/bin/env python

#####################################################
# Jenkins control script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.04.09
#
# Description:
#
#   Monitor a jenkins job, and light gummy bear lamps
#   according to the build status:
#       green: success
#       yellow: running job
#       red: failure
#
#   Based on the Traffic-light scheme described in
#       https://wiki.jenkins-ci.org/pages/viewpage.action?pageId=20250625
#
#   Run this script on the jenkins server: It will run indefinitely.
#
#   Hostnames, jobnames, passwords etc are hard-coded,
#   modify them to the task at hand.
#
#   Thanks to https://github.com/jensjaeger/PowerControl for
#   providing an interface to the NET-PwrCtrl box.
#
#   The hardware can be found here:
#       http://www.anel-elektronik.de/english/Produkte/NET-PwrCtrl_HOME/net-pwrctrl_home.html
#
#   How to get the jenkinsapi (on the command line):
#	sudo apt-get install python-setuptools (ubuntu)
#       sudo easy_install pip
#       sudo pip install jenkinsapi
#
#
#####################################################

import logging
import time    
import subprocess
import sys
import optparse
from jenkinsapi import api

class PowerControl:
    '''
    Control the power sockets.
    '''
    def __init__(self):
        #self.pwr_hostname = '192.168.0.136'
        self.pwr_hostname = '169.254.75.19'
	self.cachedValues = {} # cache values in order to set only when changed
	self.dummy = False

    def switchGreen(self, value):
        'set green socket to value'
        self.switch(1, value)
        
    def switchYellow(self, value):
        'set yellow socket to value'
        self.switch(2, value)
        
    def switchRed(self, value):
        'set red socket to value'
        self.switch(3, value)

    def switchColors(self, colors):
	for key,value in colors:
	    socketIndex = self.convertNameToSocketIndex(key)
	    self._cacheSocketValueReturnIfChanged(socketIndex, value)
	pass

    def switch(self, socketIndex, socketValue):
        '''
        Set power socket with index={1,2,3} to True or False,
        meaning on or off.
        '''
	value = self._convertBooleanToOnOff(socketValue)
        index = socketIndex

	if not self._cacheSocketValueReturnIfChanged(index, value):
	    return
        
        cmd = 'java -jar ./PowerControl.jar -c SWITCH -h %s -l admin -p anel -o %i=%s' % (self.pwr_hostname, index, value)
        self._runShell(cmd)

    def _cacheSocketValueReturnIfChanged(self, index, value):
	''
	cache = self.cachedValues
	if index in cache and cache[index] == value:
	    return False
	cache[index] = value
	print cache
	return True

    def _convertBooleanToOnOff(self, value):
	''
        if value == True:
            return 'ON'
	else:
	    return 'OFF'

    def _runShell(self, cmd):
        '''This function runs shell in cwd, no return'''
	if self.dummy:
	        print '*** dummy run:', cmd
	else:
	        print '*** run:', cmd
	        p = subprocess.Popen(cmd, shell=True, cwd=None)
    


class JenkinsGummyBears():
    '''
    Controller for monitoring jenkins and updating
    the power sockets / gummy bears.
    '''
    def __init__(self):
        self.hostname = 'http://christiana-ubuntu-desktop:8080'
        self.username = 'christiana'
        self.jobname = 'CustusX'
	self.password = 'not set'
	self.debug_counter = 0
        self.lamp = PowerControl()

    def initializeJenkins(self):
        print 'Initializing jenkins python controller ...'
	self._printConfigInfo()
        
        logging.basicConfig(filename='myapp.log', level=logging.DEBUG)
        self.jenkins = api.Jenkins(self.hostname, self.username, self.password)

        print 'jenkins python controller initialized'
	self._printSetupInfo()   
	print '='*40
	 
        
    def loop(self):
        '''
        Loop indefinitely,
        poll jenkins and update power sockets.
        '''
        while True:
	    self.setState()
            time.sleep(3)

    def testLoop(self):
        '''
        Loop indefinitely,
        turn lamp on and off, for test.
        '''
        while True:
	    self._dummySetGummyBears()
            time.sleep(3)

    def setState(self):
        '''
	Set the gummy bears and print status, once
        '''
	self._printStatus()
	self._setGummyBears()

    def getJob(self):
        '''
	Return the active job.
	NOTE:
	  Call this for every use, otherwise the 
	  build values will not be updated.
        '''
        return self.jenkins.get_job(self.jobname)

    def _printConfigInfo(self):
        ''
        print '  Hostname: ', self.hostname
        print '  Username: ', self.username
        print '  Password: ', self.password

    def _printSetupInfo(self):
        ''
        print '  Authorization: ', self.jenkins.get_jenkins_auth()
        print '  Available jobs: ', self.jenkins.get_jobs_list()
        print '  Connected to job: ', self.getJob()

    def _printStatus(self):
	job = self.getJob()
        lastBuild = job.get_last_build()
        completedBuild = job.get_last_completed_build()
        indent = '    '
        print indent, 'Checking last build: ', lastBuild
        #print indent, '        Timestamp: ', build.get_timestamp()
        print indent, '        Running: ', lastBuild.is_running()
        print indent, '        Status:  ', lastBuild.get_status()
        print indent, 'Checking last completed build: ', completedBuild
        print indent, '        Good: ', completedBuild.is_good()
        print indent, '        Status:  ', completedBuild.get_status()

    def _setGummyBears(self):
        '''
        Light the {green, yellow, red} gummy bears
        according to input from the last jenkins build.
        '''
	job = self.getJob()
        lastBuild = job.get_last_build()
        completedBuild = job.get_last_completed_build()

        green = completedBuild.is_good()
        yellow = lastBuild.is_running()
        red = not completedBuild.is_good()
        
        self.lamp.switchGreen(green)
        self.lamp.switchYellow(yellow)
        self.lamp.switchRed(red)

    def _dummySetGummyBears(self):
        '''
        For testing: use the three bears as a binary counter 0-7
        '''
	self.debug_counter += 1
	self.debug_counter = (self.debug_counter % 8)
	#print '	self.debug_counter ', self.debug_counter
	green = self.debug_counter & 0b001
	yellow = self.debug_counter & 0b010
	red = self.debug_counter & 0b100
	print "Count: %i, Colors: (%i, %i, %i) " % (self.debug_counter, green, yellow, red)

        green = green > 0
        red = red > 0
        yellow = yellow > 0
	colors = {'green':green, 'red':red, 'yellow':yellow}

        #self.lamp.switchColors(colors)

        self.lamp.switchGreen(green > 0)
        self.lamp.switchRed(red > 0)
        self.lamp.switchYellow(yellow > 0)



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
    
    def _createOptionParser(self):
        description='Controller script for Gummy Bears Extreme Feedback device.'
        
        p = optparse.OptionParser(description=description,
                                    version='%prog version 0.1',
                                    usage= '%prog [options] [components]')
        p.add_option('--username',
                     '-u',
                     action='store',
                     type='string',
                     help='jenkins user',
                     #dest='password',
                     default="user")
        p.add_option('--password',
                     '-p',
                     action='store',
                     type='string',
                     help='jenkins password',
                     default="not set")
        p.add_option('--test_bears', '-t',
                     action='store_true',
                     help='run a test sequence on the gummy bears',
                     default=False)
        p.add_option('--dummy_bears', '-d',
                     action='store_true',
                     help='run without calling lighting the bears',
                     default=False)
        p.add_option('--silent_mode', '-s',
                     action='store_true',
                     help='execute script without user interaction',
                     default=False)
        return p
    
    def run(self):
        options, arguments = self.optionParser.parse_args()
        gummybears = JenkinsGummyBears()
	gummybears.lamp.dummy = options.dummy_bears

	if options.test_bears == True:
		gummybears.testLoop()
	else:
	        gummybears.username = options.username
	        gummybears.password = options.password
		gummybears.initializeJenkins()
		gummybears.loop()

def main():
    controller = Controller()
    controller.run()

if __name__ == '__main__':
    main()
