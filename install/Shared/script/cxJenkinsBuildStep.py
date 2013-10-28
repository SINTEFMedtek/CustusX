#!/usr/bin/env python

#####################################################
# Jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
# Description:
#
#   Continous integration build: run every time the repo changes.
#   Build part of jenkins CI
#       Download, build, and test CustusX
#       Publish unit tests
#
#####################################################

import cx.cxJenkinsBuildScriptBase
import cx.cxCustusXBuilder

class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBase):
    '''
    '''
    def getDescription(self):                  
        return 'Jenkins script for build, unit test and package creation of CustusX and dependents.'
            
    def run(self):
        self.createUnitTestedPackageStep()
        self.cxBuilder.finish()                     

if __name__ == '__main__':
    controller = Controller()
    controller.run()
