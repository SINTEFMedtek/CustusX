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
import shlex

class CustusXTestInstallation:
    '''
    Represents one installed version of CustusX,
    along with functionality for testing it.
    '''
    def __init__(self):
        pass
    def setRootDir(self, root_dir):
        'root dir for user data'
        self.root_dir = root_dir
        if platform.system() == 'Darwin':
            self.install_root = '/Applications'
        if platform.system() == 'Linux':
            self.install_root = '%s/Installed' % self.root_dir

    def setTestDataPath(self, path):
        'set location of test data'
        self.test_data_path = path
        
    def getTestDataPath(self):
        return self.test_data_path
    
    def setInstallerPath(self, path):
        'set location of installer package (dmg, msi, gz) that should be installed'
        self.installer_path = path

#    def _getInstalledRootPath(self):
#        'path to root folder of install'
#        return self.install_root

    def _getInstalledBinaryPath(self):
        'path to binary files / executables in install'
        if platform.system() == 'Darwin':
            retval = '%s/CustusX/CustusX.app/Contents/MacOS' % self.install_root
        if platform.system() == 'Linux':
            retval = '%s/CustusX/bin' % self.install_root
        return retval        

    def _getInstalledSettingsPath(self):
        return '%s/CustusX/config/settings' % self.install_root
        
    def installPackage(self):
        PrintFormatter.printHeader('Install package', level=3)
        pattern = self._getInstallerPackagePattern()
        print 'Looking for installers with pattern: %s' % pattern 
        files = glob.glob(pattern)
        self.assertTrue(len(files)==1, 
                        'Found %i install files, requiring 1: \n pattern: %s\n Found:\n %s' % 
                        (len(files), pattern, ' \n'.join(files)))
        file = files[0]
        print 'Installing file %s' % file
        self._installFile(file)
        
    def _installFile(self, filename):
        if platform.system() == 'Darwin':
            self._installDMG(filename)
        if platform.system() == 'Linux':
            self._installLinuxZip(filename)
    
    def _getInstallerPackagePattern(self):
        if platform.system() == 'Darwin':
            pattern = 'CustusX_*.dmg'
        if platform.system() == 'Linux':
            pattern = 'CustusX*.tar.gz'
        return '%s/%s' % (self.installer_path, pattern)
        
    def _installLinuxZip(self, filename):
        temp_path = '%s/temp/Install' % self.root_dir
        #install_root
        shell.removeTree(temp_path)
        shell.changeDir(temp_path)
        shell.run('tar -zxvf %s' % (filename)) # extract to path
        corename = os.path.basename(filename).split('.tar.gz')[0]
        unpackedfolder = "%s/%s" % (temp_path,corename)
        installfolder = '%s' % self.install_root
        shell.changeDir(installfolder)
        shell.run('cp -r %s/* %s' % (unpackedfolder, installfolder))
        print 'Installed \n\t%s\nto folder \n\t%s ' % (filename, installfolder)

    def _installDMG(self, dmgfile, pkgName=None):
        '''
        Install the given pkg inside the dmg file.
        '''
        path = os.path.dirname(dmgfile)
        basename = os.path.basename(dmgfile)
        changeDir(path)
        coreName = os.path.splitext(basename)[0]
        if not pkgName:
            pkgName = coreName + '.pkg'
        print "install package %s from file %s" % (pkgName, coreName)
        shell.run('hdiutil attach -mountpoint /Volumes/%s %s' % (coreName, dmgfile))
        target = '/' # TODO: mount on another (test) volume - this one requires sudo
        shell.run('sudo installer -pkg /Volumes/%s/%s -target %s' % (coreName, pkgName, target))
        shell.run('hdiutil detach /Volumes/%s' % coreName)
        print "Installed %s" % pkgName
   
    def testInstallation(self):
        PrintFormatter.printHeader('Test installation', level=2)
        appPath = self._getInstalledBinaryPath()
        self._testExecutable(appPath, 'CustusX')
        self._testExecutable(appPath, 'OpenIGTLinkServer')
        self._testExecutable(appPath, 'Catch', '-h')
        if platform.system() == 'Darwin':
            self._testExecutable(appPath, 'GrabberServer')
                
    def _testExecutable(self, path, filename, arguments=''):
        PrintFormatter.printHeader('Test executable %s' % filename, level=3)
        fullname = '%s/%s' % (path, filename)
        self.assertTrue(os.path.exists(fullname), 
                        'Checking existence of installed executable %s' % fullname)
        cmd = '%s %s' % (fullname, arguments)
        self._runApplicationForDuration(cmd, timeout=3)

    def _runApplicationForDuration(self, application, timeout):
        '''
        Run the given application for a short time, as a quick verification.
        The stdout is not redirected here, i.e. it might be mangled with the python output.
        '''
        print '*** Running application %s' % application
        startTime = time.time()
        # On linux, cannot run from shell (http://stackoverflow.com/questions/4789837/how-to-terminate-a-python-subprocess-launched-with-shell-true)
        p = subprocess.Popen(shlex.split(application), shell=False, cwd=os.path.dirname(application)) 
        self._waitForProcessEnd(process=p, timeout=timeout)
        retcode = p.poll()
        elapsedTime = "%.1f" % (time.time()-startTime)
        self.assertTrue(retcode==None or retcode==0, 'Process %s has been running successfully for %ss' % (application, elapsedTime))
        if retcode==None:
            print "*** Killing %s ..." % application
            p.kill()
        p.communicate()
        print '*** Successfully ran %s for %ss' % (application, elapsedTime)
        # also consider otool -L
    
    def _waitForProcessEnd(self, process, timeout):
        resolution=10
        for interval in range(resolution):
            retcode = process.poll()
            if retcode!=None:
                break
            time.sleep(float(timeout)/resolution)
                        
    def assertTrue(self, assertion, text):
        if not assertion:
            text = 'Test Failed: %s' % text
            print text
            raise Exception(text)

    def runIntegrationTests(self):
        PrintFormatter.printHeader('Run integration tests', level=2)
        appPath = self._getInstalledBinaryPath()
        self._connectTestDataToInstallation()
        #catchFile = '%s/Catch' % appPath
        resultsFile = '%s/CatchTestResults.xml' % self.root_dir
        
        shell.changeDir(appPath)
        shell.run('rm -rf %s' % resultsFile)
        pathToCatchExe = '.'
        shell.run('%s/Catch -g [integration] -r junit -o %s' % (appPath,resultsFile))
        #shell.run('%s/Catch -g [integration] ' % (appPath))
        # todo: copy to jenkins-readable pos -NA already did this above       
        #shell.run('cp CatchTestResults.xml %s/CatchTestResults.xml' % custusx.buildPath())
    
    def _connectTestDataToInstallation(self):
        settingsPath = self._getInstalledSettingsPath()
        dataLocationFile = '%s/data_root_location.txt' % settingsPath
        self._writeToNewFile(filename=dataLocationFile, text=self.getTestDataPath())
        self.assertTrue(os.path.exists(self.getTestDataPath()), 'Looking for installed data path.')
    
    def _writeToNewFile(self, filename, text):
        path = os.path.dirname(filename)
        if not os.path.exists(path):
            os.makedirs(path)
        with open(filename, 'w') as f:
            f.write(text)



