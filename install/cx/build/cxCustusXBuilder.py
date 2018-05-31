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

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter
import cxInstallData
import cxComponents
import cxComponentAssembly
import cxTestRunner
import cxCustusXInstaller
import cx.utils.cxClean_sloccount

class CustusXBuilder:
    '''
    Utilities for building and testing CustusX.
    '''
    pass
    def __init__(self, assembly):
        ''
        self.assembly = assembly
    
    def buildAllComponents(self):
        self.assembly.controlData.printSettings()
        PrintFormatter.printHeader('Build all components', level=2)
        self.assembly.selectAllLibraries()
        self.assembly.process(checkout=True,
                             configure=True,
                             build=True)
    
    def clearTestData(self):
        PrintFormatter.printHeader('Clearing all old test data', level=3)
        cxData = self._createComponent(cxComponents.CustusXData)
        #custusx = self._createComponent(cxComponents.CustusX)
        testRunner = self._getTestRunner()
        testRunner.resetCustusXDataRepo(cxData.sourcePath())  
        testRunner.removeResultFiles(outPath=self._getTestResultsPath())
        #testRunner.removeResultFiles(outPath=custusx.buildPath())
    
    def _getTestResultsPath(self):
        testRunner = self._getTestRunner()
        return testRunner.generateOutpath(self.assembly.controlData.getRootDir())
    
    def runUnitTests(self):
        PrintFormatter.printHeader('Run all unit tests', level=2)
        self.clearTestData()
        self._runCatchUnitTests()
        self._runCTestUnitTests()
    
    def _runCTestUnitTests(self):
        PrintFormatter.printHeader('Run ctest tests', level=3)
        # Run all tests and write them in xml format to ctest.unit.*.xml,
        # ctest.xml means ctest format, junit.xml means junit format
        custusx = self._createComponent(cxComponents.CustusX)
        testRunner = self._getTestRunner()
        basename = 'ctest.unit.testresults'
        outpath = self._getTestResultsPath()
        ctestfile = '%s/%s.ctest.xml' % (outpath, basename)
        testRunner.runCTest(custusx.buildPath(), outfile=ctestfile)
        junitfile='%s/%s.junit.xml' % (outpath, basename)
        testRunner.convertCTestFile2JUnit(ctestfile, junitfile)

    def _getTestRunner(self):
        return cxTestRunner.TestRunner(target_platform=self.assembly.controlData.getTargetPlatform())

    def _runCatchUnitTests(self):
        tags = self._getTestRunner().includeTagsForOS('[unit]~[unstable]')
        self.runCatchTests(tags)

    def runCatchTests(self, tag):
        PrintFormatter.printHeader('Run catch tests using tag %s' % tag, level=3)
        # Run all Catch tests and write them in xml format to ./Catch.<tagname>.TestResults.xml
        custusx = self._createComponent(cxComponents.CustusX)
        catchDir = '%s/bin' % custusx.buildPath()
        outpath = self._getTestResultsPath()
        self._getTestRunner().runCatch(catchDir, tag=tag, outpath=outpath)
        
    def runCatchTestsWrappedInCTest(self, tag):
        PrintFormatter.printHeader('Run catch tests wrapped in ctest', level=2)
        custusx = self._createComponent(cxComponents.CustusX)
        appPath = '%s/bin' % custusx.buildPath()
        outpath = self._getTestResultsPath()
        testRunner = self._getTestRunner()
        testRunner.runCatchTestsWrappedInCTestGenerateJUnit(tag, appPath, outpath)

    def createInstallerPackage(self):
        PrintFormatter.printHeader('Package the build', level=2)
        self.removePreviousInstaller()
        custusx = self._createComponent(cxComponents.CustusX)
        shell.changeDir(custusx.buildPath())
        # run cpack directly instead of make package: this avoids the overhead of a extra make, 
        # which can be time-consuming for f.ex. doxygen genersaion
        shell.run('cpack')
        #if(self.assembly.controlData.getCMakeGenerator() == 'Eclipse CDT4 - Ninja'):
        #    shell.run('ninja package')
        #elif(platform.system() == 'Windows'):
        #    shell.run('jom package')
        #else:
        #    shell.run('make package')

        self._movePackageToStandardLocation()
        self._moveUserDocsToStandardLocation()
        self._moveDevDocsToStandardLocation()

    def _movePackageToStandardLocation(self):
        installer = self.createInstallerObject(installer_path=self._getInitialInstallerPackagePath())
        #filepattern = installer.getInstallerPackagePattern()
        source = installer.findInstallerFile()
        dest = '%s/%s' % (self._getStandardInstallerPackagePath(), os.path.basename(source))
        PrintFormatter.printInfo('Copying package files from [%s] to [%s]'%(source,dest))
        shell.cp(source, dest)

    def _moveUserDocsToStandardLocation(self):
        installer_path=self._getInitialInstallerPackagePath()
        source = '%s/%s' % (installer_path, "doc/html_pure")
        if os.path.exists(source):
            dest = '%s/%s' % (self._getStandardInstallerPackagePath(), os.path.basename(source))
            PrintFormatter.printInfo('Copying user doc files from [%s] to [%s]'%(source,dest))
            shutil.copytree(source, dest)

    def _moveDevDocsToStandardLocation(self):
        installer_path=self._getInitialInstallerPackagePath()
        source = '%s/%s' % (installer_path, "doc/html_dev")
        if os.path.exists(source):
            dest = '%s/%s' % (self._getStandardInstallerPackagePath(), os.path.basename(source))
            PrintFormatter.printInfo('Copying dev doc files from [%s] to [%s]'%(source,dest))
            shutil.copytree(source, dest)

    def createInstallerObject(self, installer_path=None):    
        if installer_path==None:
            installer_path = self._getStandardInstallerPackagePath()
        custusx = self.assembly.getComponent(cxComponents.CustusX)
#        custusxsetup = self.assembly.getComponent(cxsetup.cxPrivateComponents.CustusXSetup)

        retval = cxCustusXInstaller.CustusXInstaller()
        retval.setRootDir(self.assembly.controlData.getRootDir())
        retval.setInstallerPath(installer_path)
        retval.setSourcePath(custusx.sourcePath())  
        retval.setTargetPlatform(self.assembly.controlData.getTargetPlatform())  
        retval.set_system_base_name(self.assembly.controlData.system_base_name)
        retval.set_release_notes_path(self.assembly.controlData.main_repo_folder, self.assembly.controlData.release_notes_relative_path)

        return retval

    def removePreviousInstaller(self):
        PrintFormatter.printHeader('Removing previous installer', 3);

        shell.removeTree(self._getStandardInstallerPackagePath()); # remove everything

        initialInstaller = self.createInstallerObject(installer_path=self._getInitialInstallerPackagePath())
        shell.rm_r(initialInstaller._getInstallerPackagePattern())
        
        standardInstaller = self.createInstallerObject()
        shell.rm_r(standardInstaller._getInstallerPackagePattern())
        
        #custusx = self._createComponent(cxComponents.CustusX)
        #shell.rm_r(custusx.buildPath(), "*.exe")
        #shell.rm_r(custusx.buildPath(), "*.dmg")
        #shell.rm_r(custusx.buildPath(), "*.tar.gz")
            
    def _getStandardInstallerPackagePath(self):
        'return the path to the installer package after it has been moved to a standard location'
        return '%s/package_artefacts' % self.assembly.controlData.getRootDir()

    def _getInitialInstallerPackagePath(self): # getInstallerPackagePath
        'return the path to the installer package as generated by cmake'
        custusx = self._createComponent(cxComponents.CustusX)
        if platform.system() == 'Windows':
            build_path = custusx.buildPath()
            if self.assembly.controlData.m32bit:
#            if "32" in build_path:
                return '%s\\_CPack_Packages\\win32\\NSIS' % build_path
            else:
                return '%s\\_CPack_Packages\\win64\\NSIS' % build_path
        else:
            return custusx.buildPath()

    def publishDocumentation(self, artefactFolder, targetFolder):
        '''
        Publish both user and developer documentation to remote server.
        targetFolder is relative to the default path
        '''
        self.publishUserDocs(artefactFolder, targetFolder)
        self.publishDeveloperDocs(artefactFolder, targetFolder)
        
    def publishDeveloperDocs(self, artefactFolder, targetFolder):
        PrintFormatter.printHeader('Publish Developer Docs to server', level=2)
        source = '%s/html_dev' %  artefactFolder
        if not os.path.exists(source):
            PrintFormatter.printInfo("Warning folder don't exist: [%s]" % source)
        target = self.assembly.controlData.publish_developer_documentation_target
        custusx = self._createComponent(cxComponents.CustusX)
        target_path = '%s/%s' % (target.path, targetFolder)        
        self.publish(source, target.server, target.user, target_path)

    def publishUserDocs(self, artefactFolder, targetFolder):
        PrintFormatter.printHeader('Publish User Docs to server', level=2)
        source = '%s/html_pure' %  artefactFolder
        if not os.path.exists(source):
            PrintFormatter.printInfo("Warning folder don't exist: [%s]" % source)
        target = self.assembly.controlData.publish_user_documentation_target
        custusx = self._createComponent(cxComponents.CustusX)
        target_path = '%s/%s' % (target.path, targetFolder)        
        self.publish(source, target.server, target.user, target_path)

    def publishCoverageInfo(self, targetFolder):
        PrintFormatter.printHeader('Publish Coverage Info to server', level=2)
        target = self.assembly.controlData.publish_coverage_info_target
        custusx = self._createComponent(cxComponents.CustusX)
        source = '%s/gcov/coverage_info' %  self.assembly.controlData.getRootDir()
        target_path = '%s/%s' % (target.path, targetFolder)        
        self.publish(source, target.server, target.user, target_path)

    def publish(self, source, server, user, target_path):
        PrintFormatter.printInfo('Publishing contents of [%s] to server [%s], remote path [%s]' % (source, server, target_path))

        transfer = cx.utils.cxSSH.RemoteFileTransfer()
        transfer.connect(server, user)
        #transfer.remote_mkdir(targetBasePath)
        transfer.remote_rmdir(target_path) # remove old content if any
        transfer.copyFolderContentsToRemoteServer(source, target_path);
        transfer.close()

    def deleteCustusXBuildFolder(self):
        PrintFormatter.printHeader('Delete all contents of CustusX build folder', level=3)
        custusx = self._createComponent(cxComponents.CustusX)
        buildDir = custusx.buildPath()
        shell.run('rm -rf %s' % buildDir)

    def resetCoverage(self):
        '''
        Initialize lcov by resetting all existing counters
        and initializing/generating counters for all files.
        '''
        PrintFormatter.printHeader('Reset gcov coverage counters', level=2)
        gcovTempDir = '%s/gcov' % self.assembly.controlData.getRootDir()
        gcovResultDir = '%s/gcov/coverage_info' % self.assembly.controlData.getRootDir()
        custusx = self._createComponent(cxComponents.CustusX)
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
        custusx = self._createComponent(cxComponents.CustusX)
        buildDir = custusx.buildPath()
        sourceDir = custusx.sourcePath()

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
        shell.run(['lcov',
                '--extract %s/cx_total.gcov "%s/source/*"' % (gcovTempDir, sourceDir),
                '--output-file %s/cx_clean0.gcov' % gcovTempDir
                ])
        filterList = ["/opt/*",
                      "/Library/Frameworks/*",
                      "/external/*",
                      "/external_code/*",
                      "/Library/*",
                      "/include/boost/*",
                      "/include/c++/*",
                      "/include/qt4/*",
                      "/usr/*",
                      "/working/ISB_DataStreaming/*",
                      "/moc*.cxx",
                      "/CustusX/build_*",
                      "/Examples/*"]
        filterList = ["/ThirdParty/*",
                      "/moc*.cxx",
                      "/Examples/*"]
        fileFilter = " ".join(filterList)
        shell.run(['lcov',
                '--remove %s/cx_clean0.gcov %s' % (gcovTempDir, fileFilter),
                '--output-file %s/cx_clean1.gcov' % gcovTempDir
                ])
        shell.run(['genhtml',
                '%s/cx_clean1.gcov' % gcovTempDir,
                '--output-directory %s' % gcovResultDir
                ])
        
    def runCppCheck(self):
        PrintFormatter.printHeader('Run CppCheck', level=2)
        custusx = self._createComponent(cxComponents.CustusX)
        sourceDir = custusx.sourcePath()
        rootDir = self.assembly.controlData.getRootDir()
        shell.run(['cppcheck',
                '--enable=all',
                '--xml-version=2',
                '-i%s/source/ThirdParty/' % sourceDir,
                '-i%s/source/plugins/org.custusx.anglecorrection/core/Tests/catch.hpp' % sourceDir,
                '-i%s/source/resource/testUtilities/cisst_code/' % sourceDir,
                '%s/source 2> %s/cppcheck-result.xml' % (sourceDir, rootDir)
                ])

    def runLineCounter(self):
        PrintFormatter.printHeader('Run Line counter SLOCCOUNT', level=2)
        custusx = self._createComponent(cxComponents.CustusX)
        sourceDir = custusx.sourcePath()
        rootDir = self.assembly.controlData.getRootDir()
        results_file = '%s/sloccount_raw.sc' % rootDir 
        shell.run(['sloccount',
                '--duplicates --wide --details',
                '%s >%s' % (sourceDir, results_file)
                ])
        
        cleaned_results_file = '%s/sloccount.sc' % rootDir 
        filter = '--remove="3rdParty/ config/ install/ /data/"'
        cx.utils.cxClean_sloccount.clean_sloccount(results_file, cleaned_results_file, filter)

#        shell.run(['%s/install/Shared/script/clean_sloccount.py' % sourceDir,
#                '--remove="3rdParty/ config/ install/ /data/"',
#                '%s/sloccount_raw.sc %s/sloccount.sc' % (rootDir, rootDir) 
#                ])
     
    def finish(self):
        PrintFormatter.finish()

    def _createComponent(self, type):
        return self.assembly.getComponent(type)
        #retval = type()
        #retval.setControlData(self.assembly.controlData)
        #return retval
