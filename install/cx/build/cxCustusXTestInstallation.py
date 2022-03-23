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

from __future__ import absolute_import
from builtins import range
from builtins import object
import logging
import time    
import subprocess
import sys
import argparse        
import glob
import platform
import shlex

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter
import cx.utils.cxUtilities
from . import cxInstallData
from . import cxComponents
from . import cxComponentAssembly
from . import cxTestRunner

   
class CustusXTestInstallation(object):
    '''
    Represents one installed version of CustusX,
    along with functionality for testing it.
    '''
    def __init__(self, target_platform, root_dir, install_root, system_base_name):
        '''
        target_platform: cxUtilities.PlatformInfo instance describing target platform
        root_dir: root dir for user data. Test results can be placed here.
        install_root: path to base of installed package, i.e. the path to the CustusX folder or similar.
        test_data_path: location of test data
        '''
        self.target_platform = target_platform
        self.root_dir = root_dir
        self.install_root = install_root
        self.system_base_name = system_base_name
        
    def getTestDataPath(self):
        return "%s/CX/CX/data" % self.root_dir

    def getLargeTestDataPath(self):
        return "%s/CX/CX/largedata" % self.root_dir

    def testInstallation(self):
        PrintFormatter.printHeader('Test installation', level=2)
        appPath = self._getInstalledBinaryPath()
        self._testExecutable(appPath, 'Catch', '-h')
        self._testExecutable(appPath, self.system_base_name) # defaults to CustusX
        #self._testExecutable(appPath, 'OpenIGTLinkServer')
        self._testExecutable(appPath, 'LogConsole')
                     
    def runUnstableTests(self):
        PrintFormatter.printHeader('Run unstable tests', level=2)
        self._runCatchTestsWrappedInCTestOnInstalled('[unstable]')

    def runUnitTests(self):
        PrintFormatter.printHeader('Run unit tests', level=2)
        self._runCatchTestsOnInstalled('[unit]~[unstable]')

    def runIntegrationTests(self):
        PrintFormatter.printHeader('Run integration tests', level=2)
        self._runCatchTestsWrappedInCTestOnInstalled('[integration]~[unstable]')

    def _runCatchTestsWrappedInCTestOnInstalled(self, tags):
        appPath = self._getInstalledBinaryPath()
        self._connectTestDataToInstallation()        
        testRunner = cxTestRunner.TestRunner(self.target_platform)
        testRunner.resetCustusXDataRepo(self.getTestDataPath())
        tags = testRunner.includeTagsForOS(tags)
        outPath = testRunner.generateOutpath(self.root_dir)
        testRunner.runCatchTestsWrappedInCTestGenerateJUnit(tags, catchPath=appPath, outPath=outPath)

    def _runCatchTestsOnInstalled(self, tags):
        appPath = self._getInstalledBinaryPath()
        self._connectTestDataToInstallation()        
        testRunner = cxTestRunner.TestRunner(self.target_platform)
        testRunner.resetCustusXDataRepo(self.getTestDataPath())
        tags = testRunner.includeTagsForOS(tags)
        outPath = testRunner.generateOutpath(self.root_dir)
        testRunner.runCatch(tag=tags, path=appPath, outpath=outPath)

    def _getInstalledBinaryPath(self):
        'path to binary files / executables in install'
        if platform.system() == 'Darwin':
            retval = '%s/%s.app/Contents/MacOS' % (self.install_root, self.system_base_name)
        if (platform.system() == 'Linux') or (platform.system() == 'Windows') :
            retval = '%s/bin' % self.install_root

        return retval        

    def _testExecutable(self, path, filename, arguments=''):
        PrintFormatter.printHeader('Test executable %s' % filename, level=3)
        fullname = '%s/%s%s' % (path, filename, self._getExecutableSuffix())
        cx.utils.cxUtilities.assertTrue(os.path.exists(fullname), 'Checking existence of installed executable %s' % fullname)
        cmd = '%s %s' % (fullname, arguments)
        self._runApplicationForDuration(cmd, timeout=3)

    def _getExecutableSuffix(self):
        if self.target_platform.get_target_platform() == 'win64':
            return '.exe'
        return ''

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
        cx.utils.cxUtilities.assertTrue(retcode==None or retcode==0, 'Process %s has been running successfully for %ss' % (application, elapsedTime))
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
        cx.utils.cxUtilities.writeToNewFile(filename=dataLocationFile, text=self.getTestDataPath())
        cx.utils.cxUtilities.assertTrue(os.path.exists(self.getTestDataPath()), 'Looking for installed data path: %s'%self.getTestDataPath())

        #Optional
        dataLocationFile = '%s/large_data_root_location.txt' % settingsPath
        cx.utils.cxUtilities.writeToNewFile(filename=dataLocationFile, text=self.getLargeTestDataPath())
    
    def _getInstalledSettingsPath(self):
        return '%s/config/settings' % self.install_root



