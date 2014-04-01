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
import os

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
    def __init__(self, target_platform):
        self.target_platform = target_platform

    def generateOutpath(self, basePath):
        path = '%s/test_results' % basePath
        shell.makeDirs(path)
        return path
    
    def removeResultFiles(self, outPath):
        shell.rm_r(outPath, 'catch.*.testresults*.xml')

    def runCatchTestsWrappedInCTestGenerateJUnit(self, tag, catchPath, outPath):
        baseName = self._createCatchBaseFilenameFromTag(tag)
        ctestFile='%s/%s.ctest.xml' % (outPath, baseName)
        junitFile='%s/%s.ctest.junit.xml' % (outPath, baseName)
        
        self.runCatchTestsWrappedInCTest(catchPath, tag=tag, outFile=ctestFile)
        self.convertCTestFile2JUnit(ctestFile, junitFile)
        return junitFile

    def runCatchTestsWrappedInCTest(self, path, tag, outFile):
        '''
        Assuming a Catch executable exist in path,
        run all tests with the given tag in separate processes
        using ctest.
        ctest files are generated in path, overwriting existing files.
        ctest-style xml results are written to outFile.
        '''
        PrintFormatter.printInfo('Run ctest tests with tag %s' % tag)
        PrintFormatter.printInfo('Convert catch tests to ctests, i.e. one test per process...')
        tests = self._readCatchTestNames(path, tag=tag)
        ctestfile = '%s/CTestTestfile.cmake'%path
        self._writeCTestFileForCatchTests(ctestfile, tests)
        self._writeDartConfigurationFile(path)
        self._writeCTestConfigurationFile(path)
        self.runCTest(path, outfile=outFile)
        shell.rm_r(ctestfile)

    def runCTest(self, path, outpath=None, outfile=None):
        'Run all ctest tests at path and write them in ctest xml format to outfile'
        if not outfile:
            outfile = '%s/CTestResults.xml' % outpath
        PrintFormatter.printInfo('Run ctest, results to %s' % outfile)
        shell.changeDir(path)
        shell.rm_r('%s/Testing/' % path, "[0-9]*")
        shell.rm_r(outfile)
        shell.run('ctest -D ExperimentalTest --no-compress-output', ignoreFailure=True)
        temp_dir = shell.head(os.path.join(path, 'Testing', 'TAG'), 1)
        shell.cp(os.path.join(path, 'Testing', temp_dir, 'Test.xml'), '%s' % outfile)

    def runCatch(self, path, tag, outpath=None, outfile=None):
        'Run all Catch tests at path and write them in junit xml format to outfile'
        if not outfile:
            baseName = self._createCatchBaseFilenameFromTag(tag)
            outfile = '%s/%s.junit.xml' % (outpath, baseName)
        PrintFormatter.printInfo('Run catch with tag %s, results to %s' % (tag, outfile))
        shell.changeDir(path)
        shell.rm_r(outfile)
        exe = self._getCatchExecutable()
        cmd = '%s/%s %s --reporter junit --out %s' % (path, exe, tag, outfile)
        result = shell.run(cmd, ignoreFailure=True)
        if result.returncode >= 0:
            PrintFormatter.printInfo('catch reported %s failing tests' % result.returncode)                        
        if result.returncode < 0:
            PrintFormatter.printInfo('catch failed with returncode %s' % result.returncode)            
            PrintFormatter.printInfo('Removing outfile %s' % outfile)            
            shell.rm_r(outfile)
            PrintFormatter.printHeader('Analyzing catch failure', 2)            
            PrintFormatter.printInfo('Running catch tests wrapped in ctest.')            
            PrintFormatter.printInfo('This should identify crashing tests.')            
            self.runCatchTestsWrappedInCTestGenerateJUnit(tag, path, outpath)

    def includeTagsForOS(self, tag):
        exclude = self._getExcludeTags()
        return '%s%s' % (tag, exclude) 
        
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

    def _createCatchBaseFilenameFromTag(self, tag):
        'generate a namestring usable as the base of a filename for test results based on tag'
        tagString = self._createFilenameFriendlyStringFromCatchTag(tag)
        baseName = 'catch.%s.testresults' % tagString
        return baseName

    def _readCatchTestNames(self, path, tag):
        catchExe = '%s/%s' % (path, self._getCatchExecutable())
        #fullTags = self.includeTagsForOS(tag)
        shell.changeDir(path)
        # note: --reporter xml depends on a custom reporter implemented in cxtestCatchImpl.cpp
        testListResult = shell.evaluate('%s %s --list-tests --reporter xml' % (catchExe, tag))
        parser = cxCatchConsoleNameListParser.CatchCustomXmlNameListParser()
        parser.read(testListResult.stdout)
        #tests = parser.getTestsForTag(tag) no longer necessary - list is already filtered
        tests = parser.getTests()
        return tests

    def _writeCTestFileForCatchTests(self, targetFile, testnames):
        PrintFormatter.printInfo('Generate ctest file %s'%targetFile)
        lines = ['# Autogenerated by CustusX test scripts',
                 '# Converts Catch unit test framework to one-test-per-process',
                 '# ctest setup.'
                   ]
        for testname in testnames:
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

    def _writeCTestConfigurationFile(self, path):
        'generate CTestCustom.cmake, needed to avoid CTest truncation of output'
        targetFile = '%s/CTestCustom.cmake' % path
        PrintFormatter.printInfo('Generate ctest file %s' % targetFile)
        site = platform.node()
        buildname = platform.platform()
        content = '''\
# Autogenerated by CustusX test scripts
# Minimal file only.

# See http://www.vtk.org/Wiki/CMake_Testing_With_CTest#Customizing_CTest for more variables

# Set output size for tests (in characters/bytes)
set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 30000)
set(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 30000)
'''
        cxUtilities.writeToNewFile(filename=targetFile, text=content)

    def _getCatchExecutable(self):
        if(platform.system() == 'Windows'):
             return "Catch.exe"
        return "Catch"
                
    def _getExcludeTags(self):
        target = self.target_platform.get_target_platform()
        return '~[not_%s]~[hide]' % target
                 
    def _createFilenameFriendlyStringFromCatchTag(self, tag):
        # if tag contains name specifier, keep only the last part, usually the tag part
        if ' ' in tag:
            tag = tag.split(' ')[-1]
        tag = tag.replace('*','-')
        tag = tag.replace('?','-')
        # [unit][resource/vis]~[not_mac] ->
        # unit.resource-vis.exclude_not_mac
        tag = tag.strip('[]')
        tag = tag.replace('][','.')
        tag = tag.replace('/','-')
        tag = tag.replace(']','.')
        tag = tag.replace('~[','ex_')
        return tag
