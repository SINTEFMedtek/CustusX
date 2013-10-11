#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.09.08
#
# Description:
#
#
#####################################################

import logging
import time    
import subprocess
import sys
import argparse        

from cx.cxShell import *
from cx.cxPrintFormatter import PrintFormatter
import cx.cxInstallData
import cx.cxComponents
import cx.cxComponentAssembly
import cx.cxCustusXBuilder
import cx.cxBuildScript
import cx.cxCustusXInstaller
import cx.cxCustusXTestInstallation


class Controller(cx.cxBuildScript.BuildScript):
    '''
    '''
    def __init__(self):
        ''
        self.cxInstaller = cx.cxCustusXInstaller.CustusXInstaller()
        super(Controller, self).__init__()

    def getDescription(self):                  
        return 'Jenkins script for creating a release folder and publishing it.'
           
    def addArgParsers(self):
        self.controlData().setBuildType("Release")
        shell.setRedirectOutput(True)

        super(Controller, self).addArgParsers()
        self.additionalParsers.append(self.getArgParser())

    def applyArgumentParsers(self, arguments):
        arguments = super(Controller, self).applyArgumentParsers(arguments)
        (self.options, arguments) = self.getArgParser().parse_known_args(arguments)
        print 'Options: ', self.options

        self._initializeInstallationObject()
        return arguments

    def getArgParser(self):
        p = argparse.ArgumentParser(add_help=False)
        p.add_argument('--skip_publish_release', action='store_true', default=False, 
                       help='Skip the publish release to server step')
        return p
 
    def _initializeInstallationObject(self):
        '''
        Initialize CustusXInstallation object with data 
        from the build process.
        '''
        assembly = self.cxBuilder.assembly                
        custusxdata = assembly.getComponent(cx.cxComponents.CustusX3Data)
        custusx = assembly.getComponent(cx.cxComponents.CustusX3)
        
        self.cxInstaller.setRootDir(assembly.controlData.getRootDir())
        self.cxInstaller.setInstallerPath(custusx.buildPath())        
        self.cxInstaller.setSourcePath(custusx.sourcePath())        

    def run(self):
        folder = self.cxInstaller.createReleaseFolder()
        if not self.options.skip_publish_release:
            self.cxInstaller.publishReleaseFolder(folder)
        self.cxBuilder.finish()
    
if __name__ == '__main__':
    controller = Controller()
    controller.run()
