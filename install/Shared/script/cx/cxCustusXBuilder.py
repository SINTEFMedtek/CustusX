#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.28
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
import glob

from cxShell import *
from cxPrintFormatter import PrintFormatter
import cxInstallData
import cxComponents
import cxComponentAssembly
import cxTestRunner
import cxCustusXInstaller

class CustusXBuilder:
    '''
    Utilities for building and testing CustusX.
    '''
    pass
    def __init__(self):
        ''
        self.assembly = cxComponentAssembly.LibraryAssembly()
    
    def buildAllComponents(self):
        self.assembly.controlData.printSettings()
        PrintFormatter.printHeader('Build all components', level=2)
        self.assembly.selectAllLibraries()
        self.assembly.process(checkout=True,
                             configure=True,
                             build=True)
    
    def clearTestData(self):
        PrintFormatter.printHeader('Clearing all old test data', level=3)
        cxData = self._createComponent(cxComponents.CustusX3Data)
        #custusx = self._createComponent(cxComponents.CustusX3)
        testRunner = cxTestRunner.TestRunner()
        testRunner.resetCustusXDataRepo(cxData.sourcePath())  
        testRunner.removeResultFiles(outPath=self._getTestResultsPath())
        #testRunner.removeResultFiles(outPath=custusx.buildPath())
    
    def _getTestResultsPath(self):
        testRunner = cxTestRunner.TestRunner()
        return testRunner.generateOutpath(self.assembly.controlData.getRootDir())
    
    def runUnitTests(self):
        PrintFormatter.printHeader('Run all unit tests', level=2)
        self.clearTestData()
        self._runCatchUnitTests()
        self._runCTestTests()
    
    def _runCTestTests(self):
        PrintFormatter.printHeader('Run ctest tests', level=3)
        # Run all tests and write them in xml format to ./CTestResults.xml
        custusx = self._createComponent(cxComponents.CustusX3)
        cxTestRunner.TestRunner().runCTest(custusx.buildPath(), outpath=self._getTestResultsPath())

    def _runCatchUnitTests(self):
        tags = cxTestRunner.TestRunner().includeTagsForOS('[unit]')
        self.runCatchTests(tags)

    def runCatchTests(self, tag):
        PrintFormatter.printHeader('Run catch tests using tag %s' % tag, level=3)
        # Run all Catch tests and write them in xml format to ./Catch.<tagname>.TestResults.xml
        custusx = self._createComponent(cxComponents.CustusX3)
        catchDir = '%s/source/testing' % custusx.buildPath()
        outpath = self._getTestResultsPath()
        cxTestRunner.TestRunner().runCatch(catchDir, tag=tag, outpath=outpath)
        
    def runCatchTestsWrappedInCTest(self, tag):
        PrintFormatter.printHeader('Run catch tests wrapped in ctest', level=2)
        custusx = self._createComponent(cxComponents.CustusX3)
        appPath = '%s/source/testing' % custusx.buildPath()
        outpath = self._getTestResultsPath()
        testRunner = cxTestRunner.TestRunner()
        testRunner.runCatchTestsWrappedInCTestGenerateJUnit(tag, appPath, outpath)

    def createInstallerPackage(self):
        PrintFormatter.printHeader('Package the build', level=2)
        self.removePreviousInstaller()
        custusx = self._createComponent(cxComponents.CustusX3)
        shell.changeDir(custusx.buildPath())
        if platform.system() == 'Windows':
            shell.run('jom package')
        else:
            shell.run('make package')
        self._movePackageToStandardLocation()        

    def _movePackageToStandardLocation(self):
        installer = self.createInstallerObject(installer_path=self._getInitialInstallerPackagePath())
        #filepattern = installer.getInstallerPackagePattern()
        source = installer.findInstallerFile()
        dest = self._getStandardInstallerPackagePath()
        PrintFormatter.printInfo('Copying package files from [%s] to [%s]'%(source,dest))
        shell.cp(source, dest)

    def createInstallerObject(self, installer_path=None):    
        custusx = self.assembly.getComponent(cxComponents.CustusX3)
        retval = cxCustusXInstaller.CustusXInstaller()
        retval.setRootDir(self.assembly.controlData.getRootDir())
        if installer_path==None:
            installer_path = self._getStandardInstallerPackagePath()
        retval.setInstallerPath(installer_path)
        retval.setSourcePath(custusx.sourcePath())  
        return retval      
    
    def removePreviousInstaller(self):
        PrintFormatter.printHeader('Removing previous installer', 3);

        initialInstaller = self.createInstallerObject(installer_path=self._getInitialInstallerPackagePath())
        shell.rm_r(initialInstaller._getInstallerPackagePattern())
        
        standardInstaller = self.createInstallerObject()
        shell.rm_r(standardInstaller._getInstallerPackagePattern())
        
        #custusx = self._createComponent(cxComponents.CustusX3)
        #shell.rm_r(custusx.buildPath(), "*.exe")
        #shell.rm_r(custusx.buildPath(), "*.dmg")
        #shell.rm_r(custusx.buildPath(), "*.tar.gz")
            
    def _getStandardInstallerPackagePath(self):
        'return the path to the installer package after it has been moved to a standard location'
        return '%s/package_artefacts' % self.assembly.controlData.getRootDir()

    def _getInitialInstallerPackagePath(self): # getInstallerPackagePath
        'return the path to the installer package as generated by cmake'
        custusx = self._createComponent(cxComponents.CustusX3)
        if platform.system() == 'Windows':
            build_path = custusx.buildPath()
            if "32" in build_path:
                return '%s\\_CPack_Packages\\win32\\NSIS' % build_path
            else:
                return '%s\\_CPack_Packages\\win64\\NSIS' % build_path
        else:
            return custusx.buildPath()
        
    def publishDoxygen(self):
        PrintFormatter.printHeader('copy/publish doxygen to medtek server (link from wiki)', level=2)
        remoteServerPath = "/Volumes/medtek_HD/Library/Server/Web/Data/Sites/Default/custusx_doxygen"
        custusx = self._createComponent(cxComponents.CustusX3)
        cmd = 'scp -r %s/doc/doxygen/html/* medtek.sintef.no:%s'
        shell.run(cmd % (custusx.buildPath(), remoteServerPath))

    def resetCoverage(self):
        '''
        Initialize lcov by resetting all existing counters
        and initializing/generating counters for all files.
        '''
        PrintFormatter.printHeader('Reset gcov coverage counters', level=2)
        gcovTempDir = '%s/gcov' % self.assembly.controlData.getRootDir()
        gcovResultDir = '%s/gcov/coverage_info' % self.assembly.controlData.getRootDir()
        custusx = self._createComponent(cxComponents.CustusX3)
        buildDir = custusx.buildPath()
        
        shell.run('rm -rf %s' % gcovTempDir)
        shell.run('rm -rf %s' % gcovResultDir)
        shell.run('mkdir -p %s' % gcovTempDir)
        shell.run('mkdir -p %s' % gcovResultDir)
        shell.run('lcov --zerocounters -directory %s' % buildDir)
        shell.run('lcov --capture --initial --directory %s --output-file %s/cx_base.gcov' % (buildDir, gcovTempDir))

    def generateCoverageReport(self):
        '''
        Given that lcov is initialized and ctest is run,
        Generate html output from the gcov data.
        '''
        PrintFormatter.printHeader('Generating coverage report', level=2)
        gcovTempDir = '%s/gcov' % self.assembly.controlData.getRootDir()
        gcovResultDir = '%s/gcov/coverage_info' % self.assembly.controlData.getRootDir()
        custusx = self._createComponent(cxComponents.CustusX3)
        buildDir = custusx.buildPath()

        shell.run(['lcov',
                '--capture',
                '--directory %s' % buildDir,
                '--output-file %s/cx_test.gcov' % gcovTempDir 
                ])
        shell.run(['lcov',
                '--add-tracefile %s/cx_base.gcov' % gcovTempDir,
                '--add-tracefile %s/cx_test.gcov' % gcovTempDir,
                '--output-file %s/cx_total.gcov' % gcovTempDir
                ])
        filterList = ["/eigen3/Eigen/*",
                      "/opt/*",
                      "/Library/Frameworks/*",
                      "/external/*",
                      "/external_code/*",
                      "/Library/*",
                      "/include/boost/*",
                      "/include/c++/*",
                      "/include/qt4/*",
                      "/usr/*",
                      "/working/Tube-Segmentation-Framework/*", 
                      "/working/ISB_DataStreaming/*", 
                      "/working/Level-Set-Segmentation/*", 
                      "/moc*.cxx",
                      "/CustusX3/build_*",
                      "/Examples/*"]
        fileFilter = " ".join(filterList)
        shell.run(['lcov',
                '--remove %s/cx_total.gcov %s' % (gcovTempDir, fileFilter),
                '--output-file %s/cx_clean.gcov' % gcovTempDir
                ])
        shell.run(['genhtml',
                '%s/cx_clean.gcov' % gcovTempDir,
                '--output-directory %s' % gcovResultDir
                ])
        
    def runCppCheck(self):
        PrintFormatter.printHeader('Run CppCheck', level=2)
        custusx = self._createComponent(cxComponents.CustusX3)
        sourceDir = custusx.sourcePath()
        rootDir = self.assembly.controlData.getRootDir()
        shell.run(['cppcheck',
                '--enable=all',
                '--xml-version=2',
                '-i%s/source/ThirdParty/' % sourceDir,
                '-i%s/source/resource/ssc_not_in_use/' % sourceDir,
                '-i%s/source/resource/testUtilities/cisst_code/' % sourceDir,
                '%s/source 2> %s/cppcheck-result.xml' % (sourceDir, rootDir)
                ])

    def runLineCounter(self):
        PrintFormatter.printHeader('Run Line counter SLOCCOUNT', level=2)
        custusx = self._createComponent(cxComponents.CustusX3)
        sourceDir = custusx.sourcePath()
        rootDir = self.assembly.controlData.getRootDir()
        shell.run(['sloccount',
                '--duplicates --wide --details',
                '%s >%s/sloccount_raw.sc' % (sourceDir, rootDir)
                ])
        shell.run(['%s/install/Shared/script/clean_sloccount.py' % sourceDir,
                '--remove="3rdParty/ config/ install/ /data/"',
                '%s/sloccount_raw.sc %s/sloccount.sc' % (rootDir, rootDir) 
                ])
     
    def finish(self):
        PrintFormatter.finish()

    def _createComponent(self, type):
        return self.assembly.getComponent(type)
        #retval = type()
        #retval.setControlData(self.assembly.controlData)
        #return retval
