#!/usr/bin/env python

#####################################################
# Jenkins script
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

import cx.cxJenkinsBuildScriptBase
import cx.cxInstallData
import cx.cxCustusXBuilder
from cx.cxShell import *

class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBase):
    def getDescription(self):                  
        return '\
Jenkins script for build, test and deployment of CustusX and dependents. \
Generates coverage and other reports.'
            
    def setDefaults(self):                
        super(Controller, self).setDefaults()
        self.controlData().setBuildType("Debug")
        self.controlData().mCoverage = True

    def run(self):
        self.cxBuilder.buildAllComponents()
        self.cxBuilder.resetCoverage()
        self.cxBuilder.runUnitTests()
        self.cxBuilder.generateCoverageReport()
        self.cxBuilder.runCppCheck()
        self.cxBuilder.runLineCounter()

if __name__ == '__main__':
    controller = Controller()
    controller.run()

