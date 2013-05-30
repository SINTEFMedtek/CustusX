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
import cx.cxJenkinsBuildScriptBase

class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBase):
    '''
    '''
    def __init__(self):
        ''
        super(Controller, self).__init__()

        data = self.cxBuilder.assembly.controlData
        data.setBuildType("Debug")        
        data.mCoverage = True

    def getDescription(self):                  
        return '\
Jenkins script for build, test and deployment of CustusX and dependents. \
Generates coverage and other reports.'
    
    def run(self):
        self.cxBuilder.buildAllComponents()
        self.cxBuilder.clearTestData()
        self.cxBuilder.resetCoverage()
        self.cxBuilder.runAllTests()
        self.cxBuilder.generateCoverageReport()
        self.cxBuilder.runCppCheck()
        self.cxBuilder.runLineCounter()
                        
if __name__ == '__main__':
    controller = Controller()
    controller.run()
