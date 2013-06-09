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
        PrintFormatter.printHeader('Clearing all old test data', level=2)
        # clear local modifications in the data folder - the tests might cause these changes
        cxData = self._createComponent(cxComponents.CustusX3Data)
        shell.changeDir(cxData.sourcePath())
        shell.run('git fetch --all')
        shell.run('git reset --hard')
        tempDir = cxData.sourcePath() + "/temp"
        shell.removeTree(tempDir)
    
    def runAllTests(self):
        PrintFormatter.printHeader('Run all tests', level=2)
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
        resultsFile = '%s/CatchTestResults.xml' % custusx.buildPath()
        shell.run('%s/Catch -r junit -o %s' % (pathToCatchExe, resultsFile))
        #shell.run('cp CatchTestResults.xml %s/CatchTestResults.xml' % custusx.buildPath())        

    def createInstallerPackage(self):
        PrintFormatter.printHeader('Package the build', level=2)
        custusx = self._createComponent(cxComponents.CustusX3)
        shell.changeDir(custusx.buildPath())
        # cleanup old - can be refactored to use python methods...
        shell.run('rm -r -f *.dmg')
        shell.run('rm -r -f *.tar.gz')
        # create new
        #shell.run('make package')
        shell.run('cpack --verbose') # same as make package, given that make has been run, but more verbose

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
        PrintFormatter.printHeader('Run CppCheck', level=2)
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
        PrintFormatter.printHeader('Run Line counter SLOCCOUNT', level=2)
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
     
    def assertTrue(self, assertion, text):
        if not assertion:
            text = 'Test Failed: %s' % text
            print text
            raise Exception(text)

    def finish(self):
        PrintFormatter.finish()

    def _createComponent(self, type):
        return self.assembly.getComponent(type)
        #retval = type()
        #retval.setControlData(self.assembly.controlData)
        #return retval
