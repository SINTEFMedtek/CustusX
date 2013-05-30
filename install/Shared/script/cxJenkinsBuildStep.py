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
import cx.cxJenkinsBuildScriptBase

class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBase):
    '''
    '''
    def __init__(self):
        ''
        super(Controller, self).__init__()

        data = self.cxBuilder.assembly.controlData        
        data.setBuildType("Release")

    def getDescription(self):                  
        return 'Jenkins script for build, test and deployment of CustusX and dependents.'
       
    def run(self):
        self.cxBuilder.buildAllComponents()
        self.cxBuilder.clearTestData()
        self.cxBuilder.runAllTests()
        self.cxBuilder.createInstallerPackage()                        

if __name__ == '__main__':
    controller = Controller()
    controller.run()
