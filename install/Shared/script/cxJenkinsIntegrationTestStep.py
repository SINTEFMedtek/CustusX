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
from cx.cxPrintFormatter import PrintFormatter
import cx.cxInstallData
import cx.cxComponents
import cx.cxComponentAssembly
import cx.cxCustusXBuilder
import cx.cxJenkinsBuildScriptBase
import cx.cxCustusXInstaller
import cx.cxCustusXTestInstallation


class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBaseBase):
    '''
    '''
    def __init__(self):
        ''
        self.cxInstaller = cx.cxCustusXInstaller.CustusXInstaller()
        self.cxInstallation = cx.cxCustusXTestInstallation.CustusXTestInstallation()
        super(Controller, self).__init__()

    def getDescription(self):                  
        return 'Jenkins script for testing an installed CustusX.'
           
    def _addArgumentParserArguments(self):
        'subclasses can add parser arguments here'
        super(Controller, self)._addArgumentParserArguments()
        p = self.argumentParser
        p.add_argument('--skip_checkout', action='store_true', default=False, help='Skip the checkout of data')
        p.add_argument('--skip_install', action='store_true', default=False, help='Skip installing the package')
        p.add_argument('--skip_tests', action='store_true', default=False, help='Skip the test step')
        p.add_argument('--b32', action='store_true', default=False, help='Build 32 bit.')
        p.add_argument('--jom', action='store_true', default=False, help='Use jom to build.')
        p.add_argument('--static', action='store_true', default=False, help='Link statically.')

    def _applyArgumentParserArguments(self, options):
        'apply arguments defined in _addArgumentParserArguments()'
        super(Controller, self)._applyArgumentParserArguments(options)
        data = self.cxBuilder.assembly.controlData        
        data.setBuildType("Release")
        if(options.jom):
            data.setCMakeGenerator("NMake Makefiles JOM")
        data.setBuild32(options.b32)
        data.setBuildShared(not options.static)
        self._initializeInstallationObject()
 
    def _initializeInstallationObject(self):
        '''
        Initialize CustusXInstallation object with data 
        from the build process.
        '''
        assembly = self.cxBuilder.assembly                
        custusxdata = assembly.getComponent(cx.cxComponents.CustusX3Data)
        custusx = assembly.getComponent(cx.cxComponents.CustusX3)
        
        self.cxInstaller.setRootDir(assembly.controlData.getRootDir())
        if platform.system() == 'Windows':
            build_path = custusx.buildPath()
            if "32" in build_path:
                self.cxInstaller.setInstallerPath('%s\\_CPack_Packages\\win32\\NSIS' % build_path)
            else:
                self.cxInstaller.setInstallerPath('%s\\_CPack_Packages\\win64\\NSIS' % build_path)
        else:
            self.cxInstaller.setInstallerPath(custusx.buildPath())
            
        self.cxInstaller.setSourcePath(custusx.sourcePath())        

        self.cxInstallation.setRootDir(assembly.controlData.getRootDir())
        self.cxInstallation.setTestDataPath(custusxdata.sourcePath())
        self.cxInstallation.setInstalledRoot(self.cxInstaller.getInstalledRoot())        

    def run(self):
        if not self.argumentParserArguments.skip_checkout:
            self._checkoutComponents()
        if not self.argumentParserArguments.skip_install:
            self.cxInstaller.installPackage()
        if not self.argumentParserArguments.skip_tests:
            self.cxInstallation.testInstallation()
            self.cxInstallation.runIntegrationTests()
        self.cxBuilder.finish()
    
    def _checkoutComponents(self):
        assembly = self.cxBuilder.assembly
        PrintFormatter.printHeader('Checkout CustusX3 and CustusX3Data', level=2)
        custusx = assembly.getComponent(cx.cxComponents.CustusX3)
        cxdata = assembly.getComponent(cx.cxComponents.CustusX3Data)

        assembly.selectLibraries([custusx.name(), cxdata.name()])
        assembly.process(checkout=True)

if __name__ == '__main__':
    controller = Controller()
    controller.run()
