#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
# Description:
#
#   Continous integration build: run every time the repo changes.
#   Experimental!
#   Build part of jenkins CI
#
#       Download, build, and test CustusX
#       Publish unit tests
#
#
#####################################################

import logging
import time    
import subprocess
import sys
import argparse        

from cx.cxShell import *
import cx.cxInstallData
import cx.cxComponents
import cx.cxComponentAssembly
import cx.cxCustusXBuilder

class JenkinsBuildScriptBaseBase(object):
    '''
    Base for all scripts working on CustusX
    '''
    def __init__(self):
        ''
        shell.setRedirectOutput(True)
        
        self.cxBuilder = cx.cxCustusXBuilder.CustusXBuilder()
        self.argumentParser = self._createArgumentParser()
        self._addArgumentParserArguments()
        
        self.argumentParserArguments = self.argumentParser.parse_args()
        self._applyArgumentParserArguments(self.argumentParserArguments)
       
    def run(self):
        raise "Not Implemented"      
    
    def getDescription(self):                  
        return 'Jenkins script for operating on CustusX'
    
    def _addArgumentParserArguments(self):
        'subclasses can add parser arguments here'
        p = self.argumentParser
        p.add_argument('--root_dir', default=None, help='specify root folder, default=None')
        p.add_argument('-d', '--dummy', action='store_true', default=False, help='execute script without calling any shell commands')
        pass

    def _createArgumentParser(self):        
        return argparse.ArgumentParser(description=self.getDescription())

    def _applyArgumentParserArguments(self, options):
        'apply arguments defined in _addArgumentParserArguments()'
        shell.setDummyMode(options.dummy)
        data = self.cxBuilder.assembly.controlData        
        data.setRootDir(options.root_dir)

class JenkinsBuildScriptBase(JenkinsBuildScriptBaseBase):
    '''
    Base for all scripts working on the full build of all components
    '''
    def __init__(self):
        super(JenkinsBuildScriptBase, self).__init__()
           
    def getDescription(self):                  
        return 'Jenkins script for build, test and deployment of CustusX and dependents.'
    
    def _addArgumentParserArguments(self):
        'subclasses can add parser arguments here'
        super(JenkinsBuildScriptBase, self)._addArgumentParserArguments()
        p = self.argumentParser
        p.add_argument('-i', '--isb_password', default="not set", help='password for ISB GE Connection module')
        p.add_argument('-j', '--threads', type=int, default=1, help='number of make threads')
        pass

    def _applyArgumentParserArguments(self, options):
        'apply arguments defined in _addArgumentParserArguments()'
        super(JenkinsBuildScriptBase, self)._applyArgumentParserArguments(options)
        data = self.cxBuilder.assembly.controlData        
        data.mISBpassword = options.isb_password
        data.threads = options.threads
