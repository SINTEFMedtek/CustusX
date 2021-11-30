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

from __future__ import print_function
from __future__ import absolute_import
from builtins import object
import logging
import time    
import subprocess
import sys
import argparse        
import glob
import platform
import shlex
import datetime

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter
import cx.utils.cxUtilities
import cx.utils.cxSSH
from . import cxInstallData
from . import cxComponents
from . import cxComponentAssembly

class CustusXInstaller(object):
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
            self.install_root = '%s/Installed' % self.root_dir
#            self.install_root = '/Applications'
        if (platform.system() == 'Linux'):
            self.install_root = '%s/Installed' % self.root_dir
        if(platform.system() == 'Windows'):
            self.install_root = '%s\\Installed' % self.root_dir

    def setTargetPlatform(self, target_platform):
        self.target_platform = target_platform  

    def set_system_base_name(self, system_base_name):
        self.system_base_name = system_base_name # ='CustusX' in the default case
        
    def getInstalledFolder(self):
        '''
        Return path to base of the installation 
        performed by installPackage().
        '''
        return self.install_root+"/" + self.system_base_name
    
    def setInstallerPath(self, path):
        'set location of installer package (dmg, msi, gz) that should be installed'
        self.installer_path = path
        
    def setSourcePath(self, source_custusx_path):
        'location of source code root'
        self.source_custusx_path = source_custusx_path

    def set_release_notes_path(self, main_repo_folder, release_notes_relative_path):
        'location of release notes'
        self.release_notes_path = main_repo_folder + release_notes_relative_path

    def removePreviousJob(self):
        'remove all stuff from previous run of the installer'
        PrintFormatter.printHeader('Removing files from previous install', 3)
        shell.rm_r('%s/%s' % (self.install_root, self.system_base_name))
        shell.rm_r('%s/%s*.%s' % (self.installer_path, self.system_base_name, self._getInstallerPackageSuffix()))
        shell.removeTree('%s/temp/Install' % self.root_dir)
        shell.removeTree('%s/%s' % (self.install_root, self.system_base_name))
                         
    def createReleaseFolder(self, publishReleaseNotes):
        '''
        Create a folder containing all the files required for a Release.
        Ready to be moved to a distribution server.
        '''
        PrintFormatter.printHeader('create local release folder', level=2)
        targetPath = self._generateReleaseFolderName()
        PrintFormatter.printInfo('Creating folder %s' % targetPath)
        #shell.run('mkdir -p %s' % targetPath)
        shell.makeDirs(targetPath)
        installerFile = self.findInstallerFile()
        self._copyFile(installerFile, targetPath)
        if publishReleaseNotes:
            releaseNotes = self.findReleaseNotes()
            self._copyFile(releaseNotes, targetPath)
        return targetPath

    def _removeLocalTags(self):    
        '''
        Remove local tags,
        this removes jenkins tags that hides our own.
        # http://stackoverflow.com/questions/1841341/remove-local-tags-that-are-no-longer-on-the-remote-repository
        '''
        PrintFormatter.printInfo('Removing local git tags ...')
        shell.changeDir(self.source_custusx_path)
        # shell.run('git tag -l | xargs git tag -d') no good on windows
        # shell.run('git fetch')
        shell.run('git fetch origin --prune --tags')
        
    def _getDateString(self):
        return '%s' % datetime.date.today().isoformat()
        
    def getTaggedFolderName(self):
        '''
        Generate a folder name based on the current detected 
        installer file.
        It is assumed to be on the form 
          <system_name>_<tag>_>platform_uid>.<suffix>
        The tagged folder name equals the <tag>.
        
        Old, discarded implementation:
        Generate a folder name based on the current git tag.
        If not on a tag, create something similar but .dirty.
        Use the CustusX tags to generate - we assume that all
        repos are in syn i.e. have the same tag.
        
        Examples: v15-04                (version 15-04)
                  v15-04-245.dtq2-dirty (untagged git checkout)
        '''
        installerFile = self.findInstallerFile()
        prefix = self.system_base_name + "_"
        suffix = "." + self._getInstallerPackageSuffix()
        
        retval = os.path.basename(installerFile)

        print("retval1:", retval)
        if retval.startswith(prefix):
            retval = retval[len(prefix): ] # remove prefix from start
        print("retval2:", retval)
        if retval.endswith(suffix):
            retval = retval[ : -len(suffix)] # remove suffix from end
        print("retval3:", retval)
        retval_split = retval.split('_')
        if len(retval_split)>1:
            retval_split = retval_split[0:-1]
        retval = '_'.join(retval_split) # remove platform_uid from end, if present (remove trailing _<stuff>)
        #retval = '_'.join(retval.split('_')[0:-1]) # remove platform_uid from end, if present (remove trailing _<stuff>)
        print("retval4:", retval)
        return retval

    def _generateReleaseFolderName(self):
        '''
        Generate a name for the folder to insert release files into.
        This is a temporary folder that can be used to publish the release.
        '''
        taggedFolderName = self.getTaggedFolderName()
        targetPath = '%s/Release/%s' % (self.installer_path, taggedFolderName)
#        targetPath = '%s/Release/%s_%s' % (self.installer_path, self.system_base_name, taggedFolderName)
        return targetPath
    
    def copyReleaseFiles(self, targetPath):
        # currently not in use (2015-02-22/CA): These files are mostly redundant (replaced by online docs), 
        # and are located in the private repos.
        # If we need to publish files along with the binaries, solve by letting cmake copy these to a specific folder, then
        # python can look there, thus moving logic to cmake and using the system-polymorphism there.
        #
        'Copy files into release folder in addition to the installer.'
        source = self.source_custusxsetup_path
        self._copyFile('%s/doc/ChangeLog.rtf' % source, targetPath)
        self._copyFile('%s/doc/CustusX_Specifications.pdf' % source, targetPath)
        self._copyFile('%s/doc/CustusX_Tutorial.pdf' % source, targetPath)
        if platform.system() == 'Darwin':
            self._copyFolder('%s/install/Apple/drivers' % source, targetPath)
            self._copyFile('%s/install/Apple/apple_install_readme.rtf' % source, targetPath)
        if platform.system() == 'Linux':
            linux_distro = 'Ubuntu'
#            self._copyFolder('%s/install/Linux/script/vga2usb' % source, targetPath)
            if linux_distro == 'Ubuntu':
                self._copyFolder('%s/install/Linux/script/Ubuntu12.04' % source, targetPath)
            if linux_distro == 'Fedora':
                self._copyFolder('%s/install/Linux/script/Fedora14' % source, targetPath)
        if platform.system() == 'Windows':
            self._copyFile('%s/install/Windows/Windows_Install_ReadMe.rtf' % source, targetPath)
        
    def _copyFolder(self, source, targetPath):
        targetFolder = os.path.split(source)[1]
        fullTargetPath = '%s/%s' % (targetPath, targetFolder)
        shutil.rmtree(fullTargetPath, True)
        shutil.copytree(source, fullTargetPath)
        PrintFormatter.printInfo("copied folder %s into %s" % (source, targetPath))
        
    def _copyFile(self, source, targetPath):
        shutil.copy2(source, targetPath)
        PrintFormatter.printInfo("copied file %s into %s" % (source, targetPath))

    def publishReleaseFolder(self, source, targetFolder, target):
        '''
        Copy a release folder to server
        '''
        PrintFormatter.printHeader('copy/publish package to medtek server', level=2)
        remoteServerPath = target.path
        
        target_path = '%s/%s/%s' % (target.path, targetFolder, self._getUserFriendlyPlatformName())
        PrintFormatter.printInfo('Publishing contents of [%s] to server [%s], remote path [%s]' % (source, target.server, target_path))
        targetBasePath = '%s/%s' % (target.path, targetFolder) # need to create parent folder explicitly

        transfer = cx.utils.cxSSH.RemoteFileTransfer()
        transfer.connect(target.server, target.user)
        transfer.remote_mkdir(targetBasePath)
        transfer.remote_rmdir(target_path) # remove old content if any
        transfer.copyFolderContentsToRemoteServer(source, target_path);
        transfer.close()
        
    def _getUserFriendlyPlatformName(self):
        'generate a platform name understandable for users.'
        name = self.target_platform.get_target_platform()
        return name.title()

    def installPackage(self):
        '''
        Install the package to the default location on this machine,
        based on root_dir if necessary.
        '''
        PrintFormatter.printHeader('Install package', level=3)
        file = self.findInstallerFile()
        PrintFormatter.printInfo('Installing file %s' % file)
        self._installFile(file)
    
    def searchForFileWithPattern(self, pattern):
        '''
        find the file matching the pattern
        '''
        PrintFormatter.printInfo('Looking for installers with pattern: %s' % pattern)
        files = glob.glob(pattern)
        cx.utils.cxUtilities.assertTrue(len(files) == 1,
                        'Found %i install files, requiring 1: \n pattern: %s\n Found:\n %s' % 
                        (len(files), pattern, ' \n'.join(files)))
        file = files[0]
        return file
    
    def findInstallerFile(self):
        '''
        Find the full name of the installer file.
        '''
        pattern = self._getInstallerPackagePattern()
        file = self.searchForFileWithPattern(pattern)
        return file

    def findReleaseNotes(self):
        '''
        Find the full name of the release notes.
        '''
        file = self.searchForFileWithPattern(self.release_notes_path)
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
        return '%s/%s*.%s' % (self.installer_path, self.system_base_name, suffix)
        
    def _getInstallerPackageSuffix(self):
        if platform.system() == 'Darwin':
            return 'dmg'
        if platform.system() == 'Linux':
            return 'tar.gz'
        if platform.system() == 'Windows':
            return 'exe'
        cx.utils.cxUtilities.assertTrue(False, 'suffix not found for OS=%s' % platform.system())
        
    def _installWindowsNSISExe(self, filename):
        installfolder = '%s\%s' % (self.install_root, self.system_base_name)
        installfolder = installfolder.replace("/", "\\")
        filename = filename.replace("/", "\\")
        cmd = ["%s" % filename, "/S", "/D=%s" % installfolder]
        shell.run(cmd, convertToString=False)
        PrintFormatter.printInfo('Installed \n\t%s\nto folder \n\t%s ' % (filename, self.install_root))

    def _installLinuxZip(self, filename):
        temp_path = '%s/temp/Install' % self.root_dir
        shell.removeTree(temp_path)
        shell.changeDir(temp_path)
        shell.run('tar -zxvf %s' % (filename))  # extract to path
        corename = os.path.basename(filename).split('.tar.gz')[0]
        unpackedfolder = "%s/%s" % (temp_path, corename)
        installfolder = '%s' % self.install_root
        shell.changeDir(installfolder)
        shell.run('cp -r %s/* %s' % (unpackedfolder, installfolder))
        PrintFormatter.printInfo('Installed \n\t%s\nto folder \n\t%s ' % (filename, installfolder))

    def _installDMG_PackageMaker(self, dmgfile, pkgName=None):
        '''
        Install the given pkg inside the dmg file.
        '''
        path = os.path.dirname(dmgfile)
        basename = os.path.basename(dmgfile)
        changeDir(path)
        coreName = os.path.splitext(basename)[0]
        if not pkgName:
            pkgName = coreName + '.mpkg'
        PrintFormatter.printInfo("install package %s from file %s" % (pkgName, coreName))
        shell.run('hdiutil attach -mountpoint /Volumes/%s %s' % (coreName, dmgfile))
        target = '/'  # TODO: mount on another (test) volume - this one requires sudo
        shell.run('sudo installer -pkg /Volumes/%s/%s -target %s' % (coreName, pkgName, target))
        shell.run('hdiutil detach /Volumes/%s' % coreName)
        PrintFormatter.printInfo("Installed %s" % pkgName)

    def _installDMG(self, dmgfile, pkgName=None):
        '''
        Install the given pkg inside the dmg file.
        '''
        path = os.path.dirname(dmgfile)
        basename = os.path.basename(dmgfile)
        changeDir(path)
        coreName = os.path.splitext(basename)[0]
        if not pkgName:
            pkgName = coreName + '.mpkg'
        PrintFormatter.printInfo("install DragNDrop package %s" % (dmgfile))

        temp_path = '%s/temp/Install' % self.root_dir   
        shell.removeTree(temp_path)
        shell.changeDir(temp_path)
        
        converted_installer = "%s/%s" % (temp_path, coreName)
        mount_point = "%s/%s" % (temp_path, coreName)

        shell.run('hdiutil convert -quiet %s -format UDTO -o %s' % (dmgfile, converted_installer))
        shell.run('hdiutil attach -quiet -nobrowse -noverify -noautoopen -mountpoint %s %s.cdr' % (mount_point, converted_installer))
#/usr/bin/hdiutil convert -quiet foo.dmg -format UDTO -o bar
#/usr/bin/hdiutil attach -quiet -nobrowse -noverify -noautoopen -mountpoint right_here bar.cdr

#        installfolder = '%s' % self.install_root
        shell.changeDir(self.getInstalledFolder())
        
        shell.run('cp -r %s/*.app %s' % (mount_point, self.getInstalledFolder()+"/"))

        shell.run('hdiutil detach %s' % mount_point)
        
        
#        shell.run('hdiutil attach -mountpoint /Volumes/%s %s' % (coreName, dmgfile))
#        target = '/'  # TODO: mount on another (test) volume - this one requires sudo
#        shell.run('sudo installer -pkg /Volumes/%s/%s -target %s' % (coreName, pkgName, target))
#        shell.run('hdiutil detach /Volumes/%s' % coreName)
        
        PrintFormatter.printInfo("Installed %s" % dmgfile)
