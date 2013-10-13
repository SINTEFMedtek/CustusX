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


class JenkinsBuildScriptBase(cx.cxBuildScript.BuildScript):
    '''
    Base script for all jenkins scripts. 
    All jenkins operations is included here, subclasses can pick elements.
    '''
    def __init__(self):
        ''
        super(JenkinsBuildScriptBase, self).__init__()
     
    def setDefaults(self):                
        super(JenkinsBuildScriptBase, self).setDefaults()
        self.controlData().setBuildType("Release")
        shell.setRedirectOutput(True)
           
    def addArgParsers(self):
        super(JenkinsBuildScriptBase, self).addArgParsers()

    def applyArgumentParsers(self, arguments):
        arguments = super(JenkinsBuildScriptBase, self).applyArgumentParsers(arguments)

        self._initializeInstallationObjects()
        return arguments
 
    def _initializeInstallationObjects(self):
        '''
        Initialize Installer and Installation objects 
        with data from the build process.
        '''
        assembly = self.cxBuilder.assembly                
        custusxdata = assembly.getComponent(cx.cxComponents.CustusX3Data)
        custusx = assembly.getComponent(cx.cxComponents.CustusX3)
        
        self.cxInstaller = cx.cxCustusXInstaller.CustusXInstaller()
        self.cxInstaller.setRootDir(assembly.controlData.getRootDir())
        self.cxInstaller.setInstallerPath(self.cxBuilder.getInstallerPackagePath())
        self.cxInstaller.setSourcePath(custusx.sourcePath())        

        self.cxInstallation = cx.cxCustusXTestInstallation.CustusXTestInstallation()
        self.cxInstallation.setRootDir(assembly.controlData.getRootDir())
        self.cxInstallation.setTestDataPath(custusxdata.sourcePath())
        self.cxInstallation.setInstalledRoot(self.cxInstaller.getInstalledRoot()) 

    def resetInstallerStep(self):
        self.cxBuilder.removePreviousInstaller()
        self.cxInstaller.removePreviousJob()

    def createUnitTestedPackageStep(self, 
                                    skip_build=False, 
                                    skip_unit_tests=False, 
                                    skip_package=False):
        if not skip_build:
            self.cxBuilder.buildAllComponents()
        if not skip_unit_tests:
            self.cxBuilder.runUnitTests()
        if not skip_package:
            self.cxBuilder.createInstallerPackage()   
        
    def integrationTestPackageStep(self, 
                                   skip_extra_install_step_checkout=False, 
                                   skip_install=False, 
                                   skip_installation_test=False, 
                                   skip_integration_test=False):
        if not skip_extra_install_step_checkout:
            self.checkoutCustusXAndData()
        if not skip_install:
            self.cxInstaller.installPackage()
        if not skip_installation_test:            
            self.cxInstallation.testInstallation()
        if not skip_integration_test:
            self.cxInstallation.runIntegrationTests()

    def createReleaseStep(self, 
                          skip_publish_release=False):
        folder = self.cxInstaller.createReleaseFolder()
        if not skip_publish_release:
            self.cxInstaller.publishReleaseFolder(folder)
    
    def checkoutCustusXAndData(self):
        'checkout only CustusX and data. Required if the first build step was not run, f.ex. during integration tests'
        assembly = self.cxBuilder.assembly
        PrintFormatter.printHeader('Checkout CustusX3 and CustusX3Data', level=2)
        custusx = assembly.getComponent(cx.cxComponents.CustusX3)
        cxdata = assembly.getComponent(cx.cxComponents.CustusX3Data)

        assembly.selectLibraries([custusx.name(), cxdata.name()])
        assembly.process(checkout=True)
    
if __name__ == '__main__':
    controller = Controller()
    controller.run()
