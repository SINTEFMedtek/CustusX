#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.09.05
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

class CustusXInstaller:
    '''
    Given a compiled CustusX package,
    Functionality for installing and publishing releases.
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

    def getInstalledRoot(self):
        '''
        Return path to base of the installation 
        performed by installPackage().
        '''
        return self.install_root
    
    def setInstallerPath(self, path):
        'set location of installer package (dmg, msi, gz) that should be installed'
        self.installer_path = path
        
    def setSourcePath(self, path):
        'location of source code root'
        self.source_path = path      
        
    def createReleaseFolder(self):
        '''
        Create a folder containing all the files required for a Release.
        Ready to be moved to a distribution server.
        '''
        PrintFormatter.printHeader('create local release folder', level=2)
        targetPath = self._generateTemporaryReleaseFolderName()
#        installerFile = self._findInstallerFile()
#        suffix = self._getInstallerPackageSuffix()
#        releaseFolderName = os.path.basename(installerFile).split('.%s'%suffix)[0]
#        targetPath = '%s/Release/%s' % (self.installer_path, releaseFolderName)
        PrintFormatter.printInfo('Creating folder %s' % targetPath)
        #os.makedirs(targetPath) - no good - complains if existing.
        shell.run('mkdir -p %s' % targetPath)
        installerFile = self._findInstallerFile()
        self._copyFile(installerFile, targetPath)
        self.copyReleaseFiles(targetPath)                        
        return targetPath
    
    def _generateTemporaryReleaseFolderName(self):
        'generate a name for the folder to insert release files into'
        installerFile = self._findInstallerFile()
        suffix = self._getInstallerPackageSuffix()
        releaseFolderName = os.path.basename(installerFile).split('.%s'%suffix)[0]
        targetPath = '%s/Release/%s' % (self.installer_path, releaseFolderName)
        return targetPath
    
    def copyReleaseFiles(self, targetPath):
        'Copy files into release folder in addition to the installer.'
        self._copyFile('%s/install/Shared/doc/ChangeLog.rtf'%self.source_path, targetPath)
        self._copyFile('%s/install/Shared/doc/CustusX_Specifications.pdf'%self.source_path, targetPath)
        self._copyFile('%s/install/Shared/doc/CustusX_Tutorial.pdf'%self.source_path, targetPath)
        if platform.system() == 'Darwin':
            self._copyFolder('%s/install/Apple/drivers'%self.source_path, targetPath)
            self._copyFile('%s/install/Apple/apple_install_readme.rtf'%self.source_path, targetPath)
        if platform.system() == 'Linux':
            linux_distro='Ubuntu'
            #shutil.copy2('%s/install/Linux/copy/*'%self.source_path, targetPath)
            #shutil.copy2('%s/install/Linux/copy/run_v2u.sh'%self.source_path, targetPath)
            #shutil.copy2('%s/install/Linux/copy/v2u'%self.source_path, targetPath)
            self._copyFolder('%s/install/Linux/script/NVIDIA'%self.source_path, targetPath)
            self._copyFolder('%s/install/Linux/script/vga2usb'%self.source_path, targetPath)
            self._copyFile('%s/install/Linux/script/programmer_setup.sh'%self.source_path, targetPath)
            self._copyFile('%s/install/Linux/script/NDIToolBox_install.sh '%self.source_path, targetPath)
            if linux_distro == 'Ubuntu':
                self._copyFile('%s/install/Linux/script/ubuntu_install_readme.sh'%self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/ubuntu_ndi_setup.sh'%self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/ubuntu_epiphan_setup.sh'%self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/ubuntu_install_packages.sh'%self.source_path, targetPath)
            if linux_distro == 'Fedora':
                self._copyFile('%s/install/Linux/script/install_packages.sh'%self.source_path, targetPath)
                self._copyFile('%s/install/Linux/copy/Fedora_Linux_Installation_Guide.pdf'%self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/epiphan_setup.sh'%self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/opencl_setup.sh'%self.source_path, targetPath)
                self._copyFile('%s/install/Shared/script/sudo_setup.sh'%self.source_path, targetPath)
        if platform.system() == 'Windows':
            self._copyFile('%s/install/Windows/Windows_Install_ReadMe.rtf'%self.source_path, targetPath)
        
    def _copyFolder(self, source, targetPath):
        targetFolder = os.path.split(source)[1]
        fullTargetPath = '%s/%s' % (targetPath, targetFolder)
        shutil.rmtree(fullTargetPath, True)
        shutil.copytree(source, fullTargetPath)
        PrintFormatter.printInfo("copied folder %s into %s" % (source, targetPath))
        
    def _copyFile(self, source, targetPath):
        shutil.copy2(source, targetPath)
        PrintFormatter.printInfo("copied file %s into %s" % (source, targetPath))

    def publishReleaseFolder(self, path):
        '''
        Copy a release folder to medtek.sintef.no
        '''
        PrintFormatter.printHeader('copy/publish package to medtek server', level=2)
        remoteServer = "medtek.sintef.no"
        remoteServerPath = "/Volumes/MedTekDisk/Software/CustusX/AutomatedReleases"
#        remoteServer = "localhost"
#        remoteServerPath = "/Users/christiana/tst/AutomatedReleases"
        targetFolder = os.path.split(path)[1]
        source = '%s/*' % path
        target = '%s/%s/%s' % (remoteServerPath, targetFolder, platform.system())
        cmd1 = 'ssh %s "mkdir -p %s"' % (remoteServer, target)
        cmd2 = 'scp -r %s/* %s:%s' % (path, remoteServer, target)
        PrintFormatter.printInfo('Publishing contents of [%s] to remote path [%s]' % (path, target))
        shell.run(cmd1)
        shell.run(cmd2)

#    def getInstallFolder(self):
#        git_description = shell.evaluate('git describe --tags')
#        os_description = 'linux_test'
#        return 'CustusX.%s.%s' % (git_description, os_description)

    def installPackage(self):
        '''
        Install the package to the default location on this machine,
        based on root_dir if necessary.
        '''
        PrintFormatter.printHeader('Install package', level=3)
        file = self._findInstallerFile()
        PrintFormatter.printInfo('Installing file %s' % file)
        self._installFile(file)
        
    def _findInstallerFile(self):
        '''
        Find the full name of the installer file.
        '''
        pattern = self._getInstallerPackagePattern()
        PrintFormatter.printInfo('Looking for installers with pattern: %s' % pattern)
        files = glob.glob(pattern)
        cxUtilities.assertTrue(len(files)==1, 
                        'Found %i install files, requiring 1: \n pattern: %s\n Found:\n %s' % 
                        (len(files), pattern, ' \n'.join(files)))
        file = files[0]
        return file

    def _installFile(self, filename):
        if platform.system() == 'Darwin':
            self._installDMG(filename)
        if platform.system() == 'Linux':
            self._installLinuxZip(filename)
    
    def _getInstallerPackagePattern(self):
        suffix = self._getInstallerPackageSuffix()
        return '%s/CustusX*.%s' % (self.installer_path, suffix)
#        if platform.system() == 'Darwin':
#            pattern = 'CustusX_*.dmg'
#        if platform.system() == 'Linux':
#            pattern = 'CustusX*.tar.gz'
#        return '%s/%s' % (self.installer_path, pattern)
        
    def _getInstallerPackageSuffix(self):
        if platform.system() == 'Darwin':
            return 'dmg'
        if platform.system() == 'Linux':
            return 'tar.gz'
        cxUtilities.assertTrue(False, 'suffix not found for OS=%s' % platform.system())

    def _installLinuxZip(self, filename):
        temp_path = '%s/temp/Install' % self.root_dir
        shell.removeTree(temp_path)
        shell.changeDir(temp_path)
        shell.run('tar -zxvf %s' % (filename)) # extract to path
        corename = os.path.basename(filename).split('.tar.gz')[0]
        unpackedfolder = "%s/%s" % (temp_path,corename)
        installfolder = '%s' % self.install_root
        shell.changeDir(installfolder)
        shell.run('cp -r %s/* %s' % (unpackedfolder, installfolder))
        PrintFormatter.printInfo('Installed \n\t%s\nto folder \n\t%s ' % (filename, installfolder))

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
        PrintFormatter.printInfo("install package %s from file %s" % (pkgName, coreName))
        shell.run('hdiutil attach -mountpoint /Volumes/%s %s' % (coreName, dmgfile))
        target = '/' # TODO: mount on another (test) volume - this one requires sudo
        shell.run('sudo installer -pkg /Volumes/%s/%s -target %s' % (coreName, pkgName, target))
        shell.run('hdiutil detach /Volumes/%s' % coreName)
        PrintFormatter.printInfo("Installed %s" % pkgName)
