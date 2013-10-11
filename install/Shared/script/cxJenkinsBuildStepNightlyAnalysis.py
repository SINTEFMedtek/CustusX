#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
# Description:
#
#    Generate lcov coverage reports.
#    And some other analysis stuff.
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
import cx.cxBuildScript

class Controller(cx.cxBuildScript.BuildScript):
    '''
    '''
    def __init__(self):
        ''
        super(Controller, self).__init__()

#        data = self.cxBuilder.assembly.controlData
#        data.setBuildType("Debug")        
#        data.mCoverage = True

    def getDescription(self):                  
        return '\
Jenkins script for build, test and deployment of CustusX and dependents. \
Generates coverage and other reports.'
    
    def addArgParsers(self):
        'subclasses can add argparse instances to self.additionalparsers here'
        self.controlData().setBuildType("Debug")        
        self.controlData().mCoverage = True
        shell.setRedirectOutput(True)

        super(Controller, self).addArgParsers()
        self.additionalParsers.append(self.controlData().getArgParser_core_build())

    def applyArgumentParsers(self, arguments):
        arguments = super(Controller, self).applyArgumentParsers(arguments)
        #self.controlData().applyCommandLine() 
        return arguments

    def run(self):
        self.cxBuilder.buildAllComponents()
        #self.cxBuilder.clearTestData()
        self.cxBuilder.resetCoverage()
        self.cxBuilder.runUnitTests()
        self.cxBuilder.generateCoverageReport()
        self.cxBuilder.runCppCheck()
        self.cxBuilder.runLineCounter()
                        
if __name__ == '__main__':
    controller = Controller()
    controller.run()

