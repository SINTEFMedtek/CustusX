#!/usr/bin/env python

#####################################################
# Jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.09.08
#
#####################################################

from cx.cxPrintFormatter import PrintFormatter
import cx.cxJenkinsBuildScriptBase
import cx.cxCustusXBuilder
import cx.cxInstallData

class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBase):
    '''
    '''
    def getDescription(self):                  
        return '\
        Jenkins script for building a release and publishing it. \
        Step I: build and test'
 
    def run(self):
        PrintFormatter.printHeader('Building release for tag "%s"' % self.controlData().git_tag, 1);
        self.resetInstallerStep()
        self.createUnitTestedPackageStep()
        self.integrationTestPackageStep(skip_extra_install_step_checkout=True)
        self.cxBuilder.finish()
    
if __name__ == '__main__':
    controller = Controller()
    controller.run()
