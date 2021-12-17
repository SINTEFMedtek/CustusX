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

from __future__ import absolute_import
import logging
import time
import subprocess
import sys
import argparse
import platform

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter
import cx.build.cxInstallData
import cx.build.cxComponents
import cx.build.cxCustusXBuilder
import cx.build.cxCustusXTestInstallation
from . import cxBuildScript

class Controller(cxBuildScript.BuildScript):
    '''
    Base script for all jenkins scripts. 
    All jenkins operations is included here, subclasses can pick elements.
    '''
    def __init__(self, assembly=None):
        ''                
        if not assembly:
            assembly = cx.build.cxComponentAssembly.LibraryAssembly()
        self.cxBuilder = cx.build.cxCustusXBuilder.CustusXBuilder(assembly)
        super(Controller, self).__init__(assembly)

     
    def setDefaults(self):                
        super(Controller, self).setDefaults()
        self.controlData().setBuildType("Release")
        self.controlData().build_user_doc = True
        shell.setRedirectOutput(True)
           
    def addArgParsers(self):
        super(Controller, self).addArgParsers()
        self.additionalParsers.append(self.controlData().getArgParser_core_build())
        self.additionalParsers.append(self.controlData().getArgParser_extended_build())
        
    def applyArgumentParsers(self, arguments):
        arguments = super(Controller, self).applyArgumentParsers(arguments)
        self._initializeInstallationObjects()
        return arguments
 
    def _initializeInstallationObjects(self):
        '''
        Initialize Installer and Installation objects 
        with data from the build process.
        '''
        assembly = self.cxBuilder.assembly                
        custusxdata = assembly.getComponent(cx.build.cxComponents.CustusXData)
        custusx = assembly.getComponent(cx.build.cxComponents.CustusX)
        
        self.cxInstaller = self.cxBuilder.createInstallerObject()

        self.cxInstallation = cx.build.cxCustusXTestInstallation.CustusXTestInstallation(
                target_platform=self.controlData().getTargetPlatform(),                                                                    
                root_dir=self.controlData().getRootDir(),
                install_root=self.cxInstaller.getInstalledFolder(),
                system_base_name=self.controlData().system_base_name)

    def runAnalyze(self):
#        self.cxBuilder.deleteCustusXBuildFolder()
        self.cxBuilder.buildAllComponents()
        self.cxBuilder.resetCoverage()
        self.cxBuilder.runUnitTests()
        self.cxBuilder.generateCoverageReport()
        self.cxBuilder.runCppCheck()
        self.cxBuilder.runLineCounter()
        self.cxBuilder.publishCoverageInfo(targetFolder = "nightly")

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
                
    def testPackageStep(self, 
                        skip_extra_install_step_checkout=False, 
                        skip_install=False, 
                        skip_unit_tests=False, 
                        skip_installation_test=False, 
                        skip_integration_test=False):
        if not skip_extra_install_step_checkout:
            self.checkoutCustusXAndData()
        if not skip_install:
            self.cxInstaller.installPackage()
        if not skip_unit_tests:
            self.cxInstallation.runUnitTests()
        if not skip_installation_test:            
            self.cxInstallation.testInstallation()
        if not skip_integration_test:
            self.cxInstallation.runIntegrationTests()

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

    def unstableTestPackageStep(self, 
                                   skip_extra_install_step_checkout=False, 
                                   skip_install=False):
        if not skip_extra_install_step_checkout:
            self.checkoutCustusXAndData()
        if not skip_install:
            self.cxInstaller.installPackage()
        self.cxInstallation.runUnstableTests()

    def publishTaggedReleaseStep(self, 
                          skip_publish_exe=False,
                          skip_publish_docs=False):
        if not skip_publish_exe:
            self.publishTaggedRelease()
        if not skip_publish_docs:
            self.publishTaggedDocumentation()

    def publishNightlyReleaseStep(self, 
                          skip_publish_exe=False,
                          skip_publish_docs=False):
        if not skip_publish_exe:
            self.publishNightlyRelease()
        if not skip_publish_docs:
            self.publishNightlyDocumentation()

    def publishTaggedRelease(self):
        '''
        '''
        targetFolder = self.cxInstaller.getTaggedFolderName()
        self._publishRelease(remoteTargetFolder=targetFolder, publishReleaseNotes=True)

    def publishNightlyRelease(self):
        '''
        '''
        self._publishRelease(remoteTargetFolder="nightly", publishReleaseNotes=False)

    def _publishRelease(self, remoteTargetFolder, publishReleaseNotes):
        '''
        '''
        source = self.cxInstaller.createReleaseFolder(publishReleaseNotes) # get path to folder containing releasable files
        target = self.controlData().publish_release_target
        # install files in source to <release_server>/<targetFolder>/<platform>
        self.cxInstaller.publishReleaseFolder(source, remoteTargetFolder, target)  

    def publishTaggedDocumentation(self):
        source = self.cxInstaller.installer_path
        self.cxBuilder.publishDocumentation(artefactFolder = source, targetFolder = self.cxInstaller.getTaggedFolderName())

    def publishNightlyDocumentation(self):
        source = self.cxInstaller.installer_path
        self.cxBuilder.publishDocumentation(artefactFolder = source, targetFolder = "nightly")
    
    def checkoutCustusXAndData(self):
        'checkout only CustusX and data. Required if the first build step was not run, f.ex. during integration tests'
        assembly = self.cxBuilder.assembly
        PrintFormatter.printHeader('Checkout libraries required for integration test', level=2)
        #custusx = assembly.getComponent(cx.build.cxComponents.CustusX)
        #cxdata = assembly.getComponent(cx.build.cxComponents.CustusXData)

        #assembly.selectLibraries([custusx.name(), cxdata.name()])
        assembly.selectTestLibraries()
        assembly.process(checkout=True)

