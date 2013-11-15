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
#   Update to latest version:
#       pip install --upgrade jenkinsapi
#
#
#####################################################

import logging
import time    
import subprocess
import sys
import argparse
from cxPowerControl import PowerControl
from jenkinsapi import api
import datetime
import urllib2

class JenkinsGummyBears():
    '''
    Controller for monitoring jenkins and updating
    the power sockets / gummy bears.
    '''
    def __init__(self):
        self.hostname = 'http://christiana-ubuntu-desktop:8080'
        self.username = 'christiana'
        self.jobnames = ['CustusX_unit', 'CustusX_integration', 'CustusX_nightly']
        self.password = 'not set'
        self.debug_counter = 0
        self.lamp = PowerControl()
        self._previousStatus = None

    def initializeJenkins(self):
        print 'Initializing jenkins python controller ...'
        print self._getTimeLineString()
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
        try:
            self._printStatus()
            self._setGummyBears()
        except urllib2.HTTPError as e:
            print 'Exception caught:'
            print e
            waittime = 5
            print 'Retrying in %is...' % waittime
            time.sleep(waittime)

    def getJob(self, name):
        '''
	    Return the active job.
	    NOTE:
        Call this for every use, otherwise the 
        build values will not be updated.
        '''
        return self.jenkins.get_job(name)

    def _printConfigInfo(self):
        ''
        print '  Hostname: ', self.hostname
        print '  Username: ', self.username
        print '  Password: ', self.password

    def _printSetupInfo(self):
        ''
        print '  Authorization: ', self.jenkins.get_jenkins_auth()
        print '  Available jobs:'
        print self._generateIndentedText(self.jenkins.get_jobs_list(), 6)
        print '  Connected to jobs:'
        print self._generateIndentedText([str(self.getJob(name)) for name in self.jobnames], 6)

    def _printStatus(self):
        text = self._generateStatusText()
        if self._previousStatus == text:
            return 
        self._previousStatus = text
        print self._getTimeLineString()
        print text

    def _getTimeLineString(self):
        return 'Time: %s' % datetime.datetime.now().isoformat(' ')

    def _generateStatusText(self):
        retval = []
        for jobname in self.jobnames:
            job = self.getJob(jobname)
            retval.append(self._generateStatusTextForJob(job))
        return '\n'.join(retval)

    def _generateStatusTextForJob(self, job):
        lastBuild = job.get_last_build()
        completedBuild = job.get_last_completed_build()
        text = ''
        text = [
        'Status for job: %s' % str(job),
        '    Last build: %s ' % lastBuild,
        '        Running: %s' % lastBuild.is_running(),
        '    Last completed build: %s ' % completedBuild,
        '        Good: %s' % completedBuild.is_good(),
        '        Status:  %s' % completedBuild.get_status()
		]
        return self._generateIndentedText(text, 4)
    
    def _generateIndentedText(self, lines, indent):
        ind = ' ' * indent
        indented = ['%s%s' % (ind, line) for line in lines]
        return  '\n'.join(indented)

    def _setGummyBears(self):
        '''
        Light the {green, yellow, red} gummy bears
        according to input from the last jenkins build.
        '''
        green = self._allCompletedJobsAreGood()
        yellow = self._anyJobsAreRunning()
        red = not green
        
        self.lamp.switchGreen(green)
        self.lamp.switchYellow(yellow)
        self.lamp.switchRed(red)
        
    def _anyJobsAreRunning(self):
        for jobname in self.jobnames:
            lastBuild = self.jenkins.get_job(jobname).get_last_build()
            if lastBuild.is_running():
                return True
        return False
    
    def _allCompletedJobsAreGood(self):
        for jobname in self.jobnames:
            completedBuild = self.jenkins.get_job(jobname).get_last_completed_build()
            if not completedBuild.is_good():
                return False
        return True

    def _dummySetGummyBears(self):
        '''
        For testing: use the three bears as a binary counter 0-7
        '''
        self.debug_counter += 1
        self.debug_counter = (self.debug_counter % 8)
        green = self.debug_counter & 0b001
        yellow = self.debug_counter & 0b010
        red = self.debug_counter & 0b100
        print "Test counter: %i, Colors: (%i, %i, %i) " % (self.debug_counter, green, yellow, red)

        colors = {'green':green>0, 'red':red>0, 'yellow':yellow>0}
        self.lamp.switchColors(colors)


class Controller(object):
    '''
    '''
    def __init__(self):
    	''
        self.optionParser = self._createOptionParser();
    
    def _createOptionParser(self):
        description='Controller script for Gummy Bears Extreme Feedback device.'
        p = argparse.ArgumentParser(description=description)
        p.add_argument('-u', '--username', default="user", help='jenkins user')
        p.add_argument('-p', '--password', default="not set", help='jenkins password')
        p.add_argument('-t', '--test_bears', action='store_true', help='run a test sequence on the gummy bears (counting in binary)')
        p.add_argument('-d', '--dummy_bears', action='store_true', help='run without actually connecting to the bears')
        #p.add_argument('-s', '--silent_mode', action='store_true', help='execute script without user interaction')
        return p
    
    def run(self):
        options = self.optionParser.parse_args()
        gummybears = JenkinsGummyBears()
        gummybears.lamp.dummy = options.dummy_bears

        if options.test_bears:
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
