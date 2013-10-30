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
import cxInstallData
import cxComponents
import cxComponentAssembly
import shlex
import cxTestRunner
import cxUtilities

   
class CustusXTestInstallation:
    '''
    Represents one installed version of CustusX,
    along with functionality for testing it.
    '''
    def __init__(self):
        pass

    def setRootDir(self, root_dir):
        'root dir for user data. Test results can be placed here.'
        self.root_dir = root_dir

    def setInstalledRoot(self, install_root):
        'path to base of installed package, i.e. where the CustusX folder is located.'
        self.install_root = install_root

    def setTestDataPath(self, path):
        'location of test data'
        self.test_data_path = path
        
    def getTestDataPath(self):
        return self.test_data_path

    def testInstallation(self):
        PrintFormatter.printHeader('Test installation', level=2)
        appPath = self._getInstalledBinaryPath()
     
        if platform.system == 'Linux':
            self._testExecutable(appPath, 'Catch', '-h')
            self._testExecutable(appPath, 'CustusX')
            self._testExecutable(appPath, 'OpenIGTLinkServer')
            
        if platform.system() == 'Darwin':
            self._testExecutable(appPath, 'Catch', '-h')
            self._testExecutable(appPath, 'CustusX')
            self._testExecutable(appPath, 'OpenIGTLinkServer')
            self._testExecutable(appPath, 'GrabberServer')
            
        if platform.system() == 'Windows':
            if "32" in appPath: #Win32
                self._testExecutable(appPath, 'Catch.exe', '-h')
                self._testExecutable(appPath, 'UltrasonixServer.exe')
            else: #Win64
                self._testExecutable(appPath, 'Catch.exe', '-h')
                self._testExecutable(appPath, 'CustusX.exe')
                self._testExecutable(appPath, 'OpenIGTLinkServer.exe')
                
    def runUnstableTests(self):
        PrintFormatter.printHeader('Run unstable tests', level=2)
        self._runCatchTestsWrappedInCTestOnInstalled('[unstable]')

    def runIntegrationTests(self):
        PrintFormatter.printHeader('Run integration tests', level=2)
        self._runCatchTestsWrappedInCTestOnInstalled('[integration]~[unstable]')

    def _runCatchTestsWrappedInCTestOnInstalled(self, tags):
        appPath = self._getInstalledBinaryPath()
        self._connectTestDataToInstallation()        
        testRunner = cxTestRunner.TestRunner()
        testRunner.resetCustusXDataRepo(self.getTestDataPath())
        tags = testRunner.includeTagsForOS(tags)
        outPath = testRunner.generateOutpath(self.root_dir)
        testRunner.runCatchTestsWrappedInCTestGenerateJUnit(tags, catchPath=appPath, outPath=outPath)

    def _getInstalledBinaryPath(self):
        'path to binary files / executables in install'
        if platform.system() == 'Darwin':
            retval = '%s/CustusX/CustusX.app/Contents/MacOS' % self.install_root
        if (platform.system() == 'Linux') or (platform.system() == 'Windows') :
            retval = '%s/CustusX/bin' % self.install_root

        return retval        

    def _testExecutable(self, path, filename, arguments=''):
        PrintFormatter.printHeader('Test executable %s' % filename, level=3)
        fullname = '%s/%s' % (path, filename)
        cxUtilities.assertTrue(os.path.exists(fullname), 'Checking existence of installed executable %s' % fullname)
        cmd = '%s %s' % (fullname, arguments)
        self._runApplicationForDuration(cmd, timeout=3)

    def _runApplicationForDuration(self, application, timeout):
        '''
        Run the given application for a short time, as a quick verification.
        The stdout is not redirected here, i.e. it might be mangled with the python output.
        '''
        if(platform.system() == 'Windows'):
            application = application.replace("\\", "/")
        PrintFormatter.printInfo('Running application %s' % application)
        startTime = time.time()
        # On linux, cannot run from shell (http://stackoverflow.com/questions/4789837/how-to-terminate-a-python-subprocess-launched-with-shell-true)
        p = subprocess.Popen(shlex.split(application), shell=False, cwd=os.path.dirname(application)) 
        self._waitForProcessEnd(process=p, timeout=timeout)
        retcode = p.poll()
        elapsedTime = "%.1f" % (time.time()-startTime)
        cxUtilities.assertTrue(retcode==None or retcode==0, 'Process %s has been running successfully for %ss' % (application, elapsedTime))
        if retcode==None:
            PrintFormatter.printInfo("Killing %s ..." % application)
            p.kill()
        p.communicate()
        PrintFormatter.printInfo('Successfully ran %s for %ss' % (application, elapsedTime))
        # also consider otool -L
    
    def _waitForProcessEnd(self, process, timeout):
        resolution=10
        for interval in range(resolution):
            retcode = process.poll()
            if retcode!=None:
                break
            time.sleep(float(timeout)/resolution)
                            
    def _connectTestDataToInstallation(self):
        settingsPath = self._getInstalledSettingsPath()
        dataLocationFile = '%s/data_root_location.txt' % settingsPath
        cxUtilities.writeToNewFile(filename=dataLocationFile, text=self.getTestDataPath())
        cxUtilities.assertTrue(os.path.exists(self.getTestDataPath()), 'Looking for installed data path.')
    
    def _getInstalledSettingsPath(self):
        return '%s/CustusX/config/settings' % self.install_root



