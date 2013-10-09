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
import datetime

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
        if (platform.system() == 'Linux'):
            self.install_root = '%s/Installed' % self.root_dir
        if(platform.system() == 'Windows'):
            self.install_root = '%s\\Installed' % self.root_dir

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
        
    def removePreviousJob(self):
        'remove all stuff from previous run of the installer'
        shell.rm_r('%s/CustusX' % self.install_root)
        shell.rm_r('%s/CustusX*.%s' % (self.installer_path, self._getInstallerPackageSuffix()))
        shell.removeTree('%s/temp/Install' % self.root_dir)
        
    def createReleaseFolder(self):
        '''
        Create a folder containing all the files required for a Release.
        Ready to be moved to a distribution server.
        '''
        PrintFormatter.printHeader('create local release folder', level=2)
        targetPath = self._generateReleaseFolderName()
        PrintFormatter.printInfo('Creating folder %s' % targetPath)
        shell.run('mkdir -p %s' % targetPath)
        installerFile = self._findInstallerFile()
        self._copyFile(installerFile, targetPath)
        self.copyReleaseFiles(targetPath)                        
        return targetPath

    def _removeLocalTags(self):    
        '''
        Remove local tags,
        this removes jenkins tags that hides our own.
        # http://stackoverflow.com/questions/1841341/remove-local-tags-that-are-no-longer-on-the-remote-repository
        '''
        PrintFormatter.printInfo('Removing local git tags ...')
        shell.changeDir(self.source_path)
        shell.run('git tag -l | xargs git tag -d')
        shell.run('git fetch')
        
    def _getDateString(self):
        return '%s' % datetime.date.today().isoformat()
        
    def _generateReleaseFolderName(self):
        'generate a name for the folder to insert release files into'
        shell.changeDir(self.source_path)
        self._removeLocalTags()    
        
        output = shell.evaluate('git describe --tags --exact-match')
        if output:
            name = output.stdout.strip() 
        else:
            output = shell.evaluate('git describe --tags')
            name = output.stdout.strip() 
            name = '%s.%s' % (name, self._getDateString())
        name = 'CustusX_%s' % name

        targetPath = '%s/Release/%s' % (self.installer_path, name)
        return targetPath
    
    def copyReleaseFiles(self, targetPath):
        'Copy files into release folder in addition to the installer.'
        self._copyFile('%s/install/Shared/doc/ChangeLog.rtf' % self.source_path, targetPath)
        self._copyFile('%s/install/Shared/doc/CustusX_Specifications.pdf' % self.source_path, targetPath)
        self._copyFile('%s/install/Shared/doc/CustusX_Tutorial.pdf' % self.source_path, targetPath)
        if platform.system() == 'Darwin':
            self._copyFolder('%s/install/Apple/drivers' % self.source_path, targetPath)
            self._copyFile('%s/install/Apple/apple_install_readme.rtf' % self.source_path, targetPath)
        if platform.system() == 'Linux':
            linux_distro = 'Ubuntu'
            #shutil.copy2('%s/install/Linux/copy/*'%self.source_path, targetPath)
            #shutil.copy2('%s/install/Linux/copy/run_v2u.sh'%self.source_path, targetPath)
            #shutil.copy2('%s/install/Linux/copy/v2u'%self.source_path, targetPath)
            self._copyFolder('%s/install/Linux/script/NVIDIA' % self.source_path, targetPath)
            self._copyFolder('%s/install/Linux/script/vga2usb' % self.source_path, targetPath)
            self._copyFile('%s/install/Linux/script/programmer_setup.sh' % self.source_path, targetPath)
            self._copyFile('%s/install/Linux/script/NDIToolBox_install.sh' % self.source_path, targetPath)
            if linux_distro == 'Ubuntu':
                self._copyFile('%s/install/Linux/script/ubuntu_install_readme.txt' % self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/ubuntu_ndi_setup.sh' % self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/ubuntu_epiphan_setup.sh' % self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/ubuntu_install_packages.sh' % self.source_path, targetPath)
            if linux_distro == 'Fedora':
                self._copyFile('%s/install/Linux/script/install_packages.sh' % self.source_path, targetPath)
                self._copyFile('%s/install/Linux/copy/Fedora_Linux_Installation_Guide.pdf' % self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/epiphan_setup.sh' % self.source_path, targetPath)
                self._copyFile('%s/install/Linux/script/opencl_setup.sh' % self.source_path, targetPath)
                self._copyFile('%s/install/Shared/script/sudo_setup.sh' % self.source_path, targetPath)
        if platform.system() == 'Windows':
            self._copyFile('%s/install/Windows/Windows_Install_ReadMe.rtf' % self.source_path, targetPath)
        
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
        targetFolder = os.path.split(path)[1]
        source = '%s/*' % path
        target = '%s/%s/%s' % (remoteServerPath, targetFolder, self._getUserFriendlyPlatformName())
        cmd1 = 'ssh %s "mkdir -p %s"' % (remoteServer, target)
        cmd2 = 'scp -r %s/* %s:%s' % (path, remoteServer, target)
        PrintFormatter.printInfo('Publishing contents of [%s] to remote path [%s]' % (path, target))
        shell.run(cmd1)
        shell.run(cmd2)
        
    def _getUserFriendlyPlatformName(self):
        'generate a platform name understandable for users.'
        name = platform.system()
        if platform.system() == 'Darwin':
            # return name + platform.mac_ver() ??
            return 'Apple'
        elif platform.system() == 'Linux':
            return platform.linux_distribution()[0]
        else:
            return platform.system()

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
        cxUtilities.assertTrue(len(files) == 1,
                        'Found %i install files, requiring 1: \n pattern: %s\n Found:\n %s' % 
                        (len(files), pattern, ' \n'.join(files)))
        file = files[0]
        return file

    def _installFile(self, filename):
        if platform.system() == 'Darwin':
            self._installDMG(filename)
        if platform.system() == 'Linux':
            self._installLinuxZip(filename)
        if platform.system() == 'Windows':
            self._installWindowsNSISExe(filename)
    
    def _getInstallerPackagePattern(self):
        suffix = self._getInstallerPackageSuffix()
        return '%s/CustusX*.%s' % (self.installer_path, suffix)
        
    def _getInstallerPackageSuffix(self):
        if platform.system() == 'Darwin':
            return 'dmg'
        if platform.system() == 'Linux':
            return 'tar.gz'
        if platform.system() == 'Windows':
            return 'exe'
        cxUtilities.assertTrue(False, 'suffix not found for OS=%s' % platform.system())
        
    def _installWindowsNSISExe(self, filename):
        installfolder = '%s\CustusX' % self.install_root
        installfolder = installfolder.replace("/", "\\")
        filename = filename.replace("/", "\\")
        cmd = ["%s" % filename, "/S", "/D=%s" % installfolder]
        shell.run(cmd, convertToString=False)
        PrintFormatter.printInfo('Installed \n\t%s\nto folder \n\t%s ' % (filename, self.install_root))

    def _installLinuxZip(self, filename):
        temp_path = '%s/temp/Install' % self.root_dir
        shell.removeTree(temp_path)
        shell.changeDir(temp_path)
        shell.run('tar -zxvf %s' % (filename)) # extract to path
        corename = os.path.basename(filename).split('.tar.gz')[0]
        unpackedfolder = "%s/%s" % (temp_path, corename)
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
