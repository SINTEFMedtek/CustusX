#!/usr/bin/env python

#####################################################
# Jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
#####################################################

import cx.cxJenkinsBuildScriptBase
import cx.cxCustusXBuilder

class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBase):
    '''
    '''
    def getDescription(self):                  
        return 'Jenkins script for test install and integration tests on CustusX.'

    def run(self):
        self.unstableTestPackageStep()
        self.cxBuilder.finish()

if __name__ == '__main__':
    controller = Controller()
    controller.run()
