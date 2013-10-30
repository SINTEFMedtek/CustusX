#!/usr/bin/env python

#####################################################
# Jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
# Description:
#
#   Build part of jenkins CI
#       Download, build, and test CustusX
#       Publish unit tests
#       Publish doxygen doc to medtek.sintef.no
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
Generates doxygen docs and publishes them onto medtek.sintef.no'
            
    def setDefaults(self):                
        super(Controller, self).setDefaults()
        self.controlData().mDoxygen = True

    def run(self):
        self.createUnitTestedPackageStep()
        self.cxBuilder.publishDoxygen()
        self.cxBuilder.finish()     
                                
if __name__ == '__main__':
    controller = Controller()
    controller.run()
