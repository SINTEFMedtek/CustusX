#!/usr/bin/env python

#####################################################
# NET-PwrCtrl controller class
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.14
#
# Description:
#   Helper class for interfacing to the NET-PwrCtrl gummy-bear controller.
#
#   Thanks to https://github.com/jensjaeger/PowerControl for
#   providing an interface to the NET-PwrCtrl box.
#
#   The hardware can be found here:
#       http://www.anel-elektronik.de/english/Produkte/NET-PwrCtrl_HOME/net-pwrctrl_home.html
#
#
#####################################################

import logging
import time    
import subprocess
import sys
import os

class PowerControl:
    '''
    Control the power sockets.
    '''
    def __init__(self):
        #self.pwr_hostname = '192.168.0.136'
        self.pwr_hostname = '169.254.75.19'

        moduleFile = os.path.realpath(__file__)
        modulePath = os.path.dirname(moduleFile)
        modulePath = os.path.abspath(modulePath)
        self.power_control_java_file = '%s/PowerControl.jar' % modulePath
        self.cachedValues = {} # cache values in order to set only when changed
        self.dummy = False
        self._changed = True

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
        '''
        Set any set of colors.
        Input a dictionary of colors and boolean values.
        '''
    	#print "input: ", colors
    	for (key,value) in colors.iteritems():
            self._setCachedSocketValue(key, value)
        self._setLightsIfChanged()

    def switch(self, socketIndex, socketValue):
        '''
        Set power socket with index={1,2,3} to True or False,
        meaning on or off.
        '''
        self.switchColors({ socketIndex:socketValue })

    def _setCachedSocketValue(self, index, value):
        index = self._convertNameToSocketIndex(index)
        value = self._convertBooleanToOnOff(value)
        cache = self.cachedValues
        if index in cache and cache[index] == value:
            return
        cache[index] = value
        self._changed = True
    
    def _convertNameToSocketIndex(self, name):
        mapper = { 'green':1, 'yellow':2, 'red':3 }
        if name in mapper.keys():
        	return mapper[name]
        else:
        	return name

    def _setLightsIfChanged(self):
    	if not self._changed:
    		return
        command = self._createSetLightsCommand()
        self._runShell(command)
        self._changed = False
	
    def _createSetLightsCommand(self):
        command = 'java -jar %s -c SWITCH -h %s -l admin -p anel' % (self.power_control_java_file, self.pwr_hostname)
        setterBase = ' -o %i=%s'
        for index, value in self.cachedValues.iteritems():
            setter = setterBase % (index, value)
            command += setter
        return command

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
            p.wait()
    

