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

from cxShell import *
import cxInstallData
import cxComponents
import cxComponentAssembly


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
        self.assembly.printHeader('Build all components', level=2)
        self.assembly.selectAllLibraries()
        self.assembly.process(checkout = True, 
                             configure_clean = False, 
                             configure = True, 
                             clean = False, 
                             build = True)
    
    def clearTestData(self):
        self.assembly.printHeader('Clearing all old test data', level=2)
        # clear local modifications in the data folder - the tests might cause these changes
        cxData = self._createComponent(cxComponents.CustusX3Data)
        shell.changeDir(cxData.sourcePath())
        shell.run('git fetch --all')
        shell.run('git reset --hard')
        tempDir = cxData.sourcePath()+"/temp"
        shell.removeTree(tempDir)
    
    def runAllTests(self):
        self.assembly.printHeader('Run all tests', level=2)
        self._runCTestTests()
        self._runCatchTests()
    
    def _runCTestTests(self):
        # Run all tests and write them in xml format to ./CTestResults.xml
        custusx = self._createComponent(cxComponents.CustusX3)
        shell.changeDir(custusx.buildPath())
        shell.run('rm -rf Testing/[0-9]*')
        shell.run('rm -rf %s/CTestResults.xml' % custusx.buildPath())
        shell.run('ctest -D ExperimentalTest --no-compress-output', ignoreFailure=True)
        shell.run('cp Testing/`head -n 1 Testing/TAG`/Test.xml ./CTestResults.xml')

    def _runCatchTests(self):
        # Run all Catch tests and write them in xml format to ./CatchTestResults.xml
        custusx = self._createComponent(cxComponents.CustusX3)
        catchDir = custusx.buildPath() + "/source/testing"
        shell.changeDir(catchDir)
        shell.run('rm -rf %s/CatchTestResults.xml' % custusx.buildPath())
        pathToCatchExe = '.'
        if platform.system() == 'Darwin':
            pathToCatchExe = './Catch.app/Contents/MacOS'
        shell.run('%s/Catch -r junit -o CatchTestResults.xml' % pathToCatchExe)
        shell.run('cp CatchTestResults.xml %s/CatchTestResults.xml' % custusx.buildPath())        

    def createInstallerPackage(self):
        self.assembly.printHeader('Package the build', level=2)
        custusx = self._createComponent(cxComponents.CustusX3)
        shell.changeDir(custusx.buildPath())
        shell.run('make package')

    def publishDoxygen(self):
        self.assembly.printHeader('copy/publish doxygen to medtek server (link from wiki)', level=2)
        remoteServerPath="/Volumes/medtek_HD/Library/Server/Web/Data/Sites/Default/custusx_doxygen"
        custusx = self._createComponent(cxComponents.CustusX3)
        cmd = 'scp -r %s/doc/doxygen/html/* medtek.sintef.no:%s'
        shell.run(cmd % (custusx.buildPath(), remoteServerPath))

    def resetCoverage(self):
        '''
        Initialize lcov by resetting all existing counters
        and initializing/generating counters for all files.
        '''
        self.assembly.printHeader('Reset gcov coverage counters', level=2)
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
        self.assembly.printHeader('Generating coverage report', level=2)
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
                      "/usr/*",
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
        self.assembly.printHeader('Run CppCheck', level=2)
        custusx = self._createComponent(cxComponents.CustusX3)
        sourceDir = custusx.sourcePath()
        rootDir = self.assembly.controlData.getRootDir()
        shell.run(['cppcheck',
                '--enable=all',
                '--xml-version=2',
                '-i%s/externals/ssc/Code/3rdParty/' % sourceDir,
                '%s/source %s/externals/ssc/Code/ 2> %s/cppcheck-result.xml' % (sourceDir, sourceDir, rootDir)
                ])

    def runLineCounter(self):
        self.assembly.printHeader('Run Line counter SLOCCOUNT', level=2)
        custusx = self._createComponent(cxComponents.CustusX3)
        sourceDir = custusx.sourcePath()
        rootDir = self.assembly.controlData.getRootDir()
        shell.run(['sloccount',
                '--duplicates --wide --details',
                '%s >%s/sloccount_raw.sc' % (sourceDir, rootDir)
                ])
        shell.run(['%s/testing/jenkins/clean_sloccount.py' % sourceDir, 
                '--remove="3rdParty/ config/ install/ /data/"',
                '%s/sloccount_raw.sc %s/sloccount.sc' % (rootDir, rootDir) 
                ])
 
    def _createComponent(self, type):
        retval = type()
        retval.setControlData(self.assembly.controlData)
        return retval
