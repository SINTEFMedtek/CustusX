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
#       sudo easy_install pip
#       sudo pip install jenkinsapi
#
#
#####################################################

import logging
import time    
import subprocess
from jenkinsapi import api

class PowerControl:
    '''
    Control the power sockets.
    '''
    def __init__(self):
        self.pwr_hostname = '192.168.0.136'
        pass

    def switchGreen(self, value):
        'set green socket to value'
        self.switch(1, value)
        
    def switchYellow(self, value):
        'set yellow socket to value'
        self.switch(2, value)
        
    def switchRed(self, value):
        'set red socket to value'
        self.switch(3, value)

    def switch(self, socketIndex, socketValue):
        '''
        Set power socket with index={1,2,3} to True or False,
        meaning on or off.
        '''
        index = socketIndex
        value = 'OFF'
        if socketValue == True:
            value = 'ON'
        
        cmd = 'java -jar ./PowerControl.jar -c SWITCH -h %s -l admin -p anel -o %i=%s' % (self.pwr_hostname, index, value)
        self._runShell(cmd)

    def _runShell(self, cmd):
        '''This function runs shell in cwd, no return'''
        print '*** run:', cmd
        p = subprocess.Popen(cmd, shell=True, cwd=None)
    

class JenkinsGummyBears():
    '''
    Controller for monitoring jenkins and updating
    the power sockets.
    '''
    def __init__(self):
        self.hostname = 'http://localhost:8080'
        self.username = 'christiana'
        self.password = 'christiana'
        self.jobname = 'MacJob'
        self.debug_start_pwr_val = False
        
        logging.basicConfig(filename='myapp.log', level=logging.DEBUG)
        self.jenkins = api.Jenkins(self.hostname, self.username, self.password)
        self.job = self.jenkins.get_job(self.jobname)
        self.lamp = PowerControl()
    
        print 'jenkins python controller initialized'
        print '  Authorization: ', self.jenkins.get_jenkins_auth()
        print '  Available jobs: ', self.jenkins.get_jobs_list()
        print '  Connected to job: ', self.job
        
    def loop(self):
        '''
        Loop indefinitely,
        poll jenkins and update power sockets.
        '''
        while True:
            self._printStatus()
            self._setGummyBears()
            #self._dummySetGummyBears()
            time.sleep(3)

    def _printStatus(self):
        build = self.job.get_last_build()
        indent = '    '
        print indent, 'Checking last build: ', build
        print indent, '  status: ', build.get_status()
        print indent, '  timestamp: ', build.get_timestamp()
        print indent, '  good: ', build.is_good()
        print indent, '  running: ', build.is_running()

    def _setGummyBears(self):
        '''
        Light the {green, yellow, red} gummy bears
        according to input from the last jenkins build.
        '''
        build = self.job.get_last_build()
        green = build.is_good() and not build.is_running()
        red = not build.is_good() and not build.is_running()
        yellow = build.is_running()
        
        self.lamp.switchGreen(green)
        self.lamp.switchYellow(yellow)
        self.lamp.switchRed(red)

    def _dummySetGummyBears(self):
        '''
        For testing: alternate the green bear on/off.
        '''
        self.lamp.switchGreen(self.debug_start_pwr_val)
        
        if self.debug_start_pwr_val==True:
            self.debug_start_pwr_val = False
        else:
            self.debug_start_pwr_val = True


def main():
    controller = JenkinsGummyBears()
    controller.loop()


if __name__ == '__main__':
    main()
