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
import platform

from cxShell import *
from cxPrintFormatter import PrintFormatter
import cxCatchConsoleNameListParser
import cxUtilities
import cxConvertCTest2JUnit

class TestRunner:
    '''
    Utilities for runnit tests,
    based on the catch, cppunit and ctest frameworks.
    '''
    pass

    def runCatchTestsWrappedInCTest(self, path, tag, outFile):
        '''
        Assuming a Catch executable exist in path,
        run all tests with the given tag in separate processes
        using ctest.
        ctest files are generated in path, overwriting existing files.
        ctest-style xml results are written to outFile.
        '''
        PrintFormatter.printInfo('Run ctest tests with tag [%s]' % tag)
        PrintFormatter.printInfo('Convert catch tests to ctests, i.e. one test per process...')
        tests = self._readCatchTestNames(path, tag=tag)
        self._writeCTestFileForCatchTests('%s/CTestTestfile.cmake'%path, tests)
        self._writeDartConfigurationFile(path)
        self.runCTest(path, outFile)

    def _readCatchTestNames(self, path, tag):
        catchExe = '%s/Catch' % path
        shell.changeDir(path)
        testListResult = shell.evaluate('%s --list-tests' % catchExe)
        parser = cxCatchConsoleNameListParser.CatchConsoleNameListParser()
        parser.readTestNamesFromStringList(testListResult.stdout.split('\n'))
        #parser.printTests("Tests")
        tests = parser.getTestsForTag(tag)
        return tests

    def _writeCTestFileForCatchTests(self, targetFile, testnames):
        PrintFormatter.printInfo('Generate ctest file %s'%targetFile)
        lines = ['# Autogenerated by CustusX test scripts',
                 '# Converts Catch unit test framework to one-test-per-process',
                 '# ctest setup.'
                   ]
        for testname in testnames:
            #line = 'ADD_TEST("%s" ./Catch "%s" --reporter xml)' % (testname,testname    )
            # generate raw output from catch instead of xml, as the stdout is processed by ctest
            line = 'ADD_TEST("%s" ./Catch "%s")' % (testname,testname)
            lines.append(line)
        cxUtilities.writeToNewFile(filename=targetFile, text='\n'.join(lines))

    def _writeDartConfigurationFile(self, path):
        'generate DartConfiguration.tcl, required by ctest'
        targetFile = '%s/DartConfiguration.tcl' % path
        PrintFormatter.printInfo('Generate ctest file %s' % targetFile)
        site = platform.node()
        buildname = platform.platform()
        timeout = 10*60 # seconds
        content = '''\
# Autogenerated by CustusX test scripts
# Minimal file only.

# Site is something like machine.domain, i.e. pragmatic.crd
Site: %s
# Build name is osname-revision-compiler, i.e. Linux-2.4.2-2smp-c++
BuildName: %s
# Testing options
# TimeOut is the amount of time in seconds to wait for processes
# to complete during testing.  After TimeOut seconds, the
# process will be summarily terminated.
TimeOut: %d
''' % (site,buildname,timeout)
        cxUtilities.writeToNewFile(filename=targetFile, text=content)

    def runCTest(self, path, outfile=None):
        'Run all ctest tests at path and write them in ctest xml format to outfile'
        if not outfile:
            outfile = '%s/CTestResults.xml' % path
        PrintFormatter.printInfo('Run ctest, results to %s' % outfile)
        shell.changeDir(path)
        shell.run('rm -rf ./Testing/[0-9]*')
        shell.run('rm -rf %s' % outfile)
        shell.run('ctest -D ExperimentalTest --no-compress-output', ignoreFailure=True)
        shell.run('cp ./Testing/`head -n 1 ./Testing/TAG`/Test.xml %s' % outfile)

    def runCatch(self, path, tag, outfile=None):
        'Run all Catch tests at path and write them in junit xml format to outfile'
        if not outfile:
            outfile = '%s/CatchTestResults.xml' % path
        PrintFormatter.printInfo('Run catch wit tag [%s], results to %s' % (tag, outfile))
        shell.changeDir(path)
        shell.run('rm -rf %s' % outfile)
        shell.run('%s/Catch [%s] --reporter junit --out %s' % (path, tag, outfile))
        #shell.run('%s/Catch [%s] --out %s' % (path, tag, outfile))
        
    def resetCustusXDataRepo(self, path):
        '''
        Reset the test data git repo,
        and delete the temp subfolder.
        This matches the structure of the CustusX git data repo.
        '''
        PrintFormatter.printInfo('Reset/Clear Data repository...')
        # clear local modifications in the data folder - the tests might cause these changes
        shell.changeDir(path)
        shell.run('git fetch --all')
        shell.run('git reset --hard')
        tempDir = "%s/temp" % path
        shell.removeTree(tempDir)
        
    def convertCTestFile2JUnit(self, ctestFile, junitFile):
        PrintFormatter.printInfo('Convert [%s] to [%s]' % (ctestFile, junitFile))
        cxConvertCTest2JUnit.convertCTestFile2JUnit(ctestFile, junitFile)
        
        

