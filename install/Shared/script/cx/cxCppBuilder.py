#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#
# Description:
#
#             
#################################################             

import subprocess
import optparse
import re
import sys
import os.path
import urllib
import getpass
import platform
import shutil

from cxShell import *
    
class CppBuilder:
    '''
    Contains methods for working on a cpp project
    '''
    def __init__(self):
        self.cmakeOptions={}
    def setPaths(self, base, build, source):
        self.mBasePath = base
        self.mBuildPath = build
        self.mSourcePath = source
    def setControlData(self, data):
        self.controlData = data
    def setBuildType(self, type):
        self.mBuildType = type
    
    def reset(self):
        'delete build folder(s)'
#        self._changeDirToBase()
        shell.removeTree(self.mBuildPath)
#        if(platform.system() == 'Windows'):
#            #runShell('echo WANT TO REMOVE FOLDER %s/%s' % (self.path(), self.buildFolder()))
#            runShell('rd /S /Q "%s/%s"' % (self.path(), self.buildFolder()))
#        else:
#            runShell('rm -R -f %s/%s' % (self.path(), self.buildFolder()))
            
    def build(self):
        self._changeDirToBuild()
        #self._changeDirToBuild()
        if(platform.system() == 'Windows'):
            if(self.controlData.mCMakeGenerator == 'Eclipse CDT4 - NMake Makefiles'):
                runShell('nmake')
            if(self.controlData.mCMakeGenerator == 'NMake Makefiles JOM'):
                runShell('''jom -k -j%s''' % str(self.controlData.threads))
        else:
            # the export DYLD... line is a hack to get shared linking to work on MacOS with vtk5.6
            # - http://www.mail-archive.com/paraview@paraview.org/msg07520.html
            # (add it to all project because it does no harm if not needed)
            runShell('''\
    export DYLD_LIBRARY_PATH=`pwd`/bin; \
    make -j%s
    ''' % str(self.controlData.threads))

    def gitClone(self, repository, folder=''):
        self._changeDirToBase()
        runShell('git clone %s %s' % (repository, folder))
        self._changeDirToSource()

#    def isValidGitRepo(self):
#        self._changeDirToSource()
#        return shell.evaluate('git status') is not None           

    def _gitSubmoduleUpdate(self):
        self._changeDirToSource()
        runShell('git submodule sync') # from tsf 
        runShell('git submodule update --init --recursive')                    

    def gitUpdate(self, branch='master', submodules=False):
        '''
        pull latest versiono of branch, include submodules if asked.
        '''
        self._changeDirToSource()

        runShell('git checkout %s' % branch)
        runShell('git pull')
        if submodules:
            self._gitSubmoduleUpdate()
        
    def gitCheckout(self, tag, patch=None, submodules=False):
        '''
        Update git to the given tag.
        Skip if HEAD already is at tag.
        If patch is given, apply the patch after updating to tag.
        '''
        self._changeDirToSource()

        checklatest = tag
        if patch:
            checklatest = patch
        if self._checkGitIsAtTag(checklatest):
            return        

        runShell('git checkout %s' % tag)
        if submodules:
            self._getBuilder()._gitSubmoduleUpdate()
        
        if patch:       
            self._gitApplyPatch(patch)     

    def _checkGitIsAtTag(self, tag):
        output = shell.evaluate('git describe --tags --exact-match')
        if not output:
            return False
        if output.strip()==tag:
            print "Skipping git update: Tag %s already at HEAD in %s" % (tag, self.mSourcePath)
            return True
        return False
            
    def _gitApplyPatch(self, patchFile):
        '''
            Howto create a patch using git:
            Branch is created like this:
            git checkout v5.8.0
            git branch cx_mod_for_5-8-0
            git checkout cx_mod_for_5-8-0
            ... make you modifications ...
            git commit -am "message"
            git format-patch master --stdout > VTK-5-8-0.patch
        '''
        self._changeDirToSource()
        branchName = patchFile + "_branch"
        shell.run('git branch -D %s' % branchName, ignoreFailure=True)
        shell.run('git checkout -B %s' % branchName)
        #TODO this can be a bug, if CustusX is not checked out yet, this will not work!!! 
        # (i.e. if patch file is not found in expected position)
        patchPath = self._getPathToModule() + "/.."
        runShell('git am --whitespace=fix --signoff < %s/%s' % (patchPath, patchFile))
        runShell('git tag -f %s' % patchFile) # need this tag to check for change during next update
        
    def _getPathToModule(self):
        # alternatively use  sys.argv[0] ?? 
        moduleFile = os.path.realpath(__file__)
        modulePath = os.path.dirname(moduleFile)
        modulePath = os.path.abspath(modulePath)
        return modulePath
                 
    def makeClean(self):
        self._changeDirToBuild()
        #self._changeDirToBuild()
        if(platform.system() == 'Windows'):
            if(self.controlData.mCMakeGenerator == 'Eclipse CDT4 - NMake Makefiles'):
                runShell('nmake -clean')
            if(self.controlData.mCMakeGenerator == 'NMake Makefiles JOM'):
                runShell('jom -clean')
        else:
            runShell('make clean')

    def addCMakeOption(self, key, value):
        self.cmakeOptions[key] = value

    def configureCMake(self, options=""):        
        self._addDefaultCmakeOptions()                
        generator = self.controlData.mCMakeGenerator
        optionsFromAssembly = self._assembleOptions()
        self._printOptions()        
        cmd = 'cmake -G"%s" %s %s %s'
        cmd = cmd % (generator, options, optionsFromAssembly, self.mSourcePath)        

        self._changeDirToBuild()
        runShell(cmd)

    def _addDefaultCmakeOptions(self):
            add = self.addCMakeOption
            add('CMAKE_CXX_FLAGS:STRING', '-Wno-deprecated')
            add('CMAKE_BUILD_TYPE:STRING', self.mBuildType)        
            if self.controlData.m32bit: # todo: add if darwin
                add('CMAKE_OSX_ARCHITECTURES', 'i386')
            add('BUILD_SHARED_LIBS:BOOL', self.controlData.getBuildShared())
#            add('BUILD_TESTING:BOOL', self.controlData.mBuildExAndTest)
#            add('BUILD_EXAMPLES:BOOL', self.controlData.mBuildExAndTest)
#            add('BUILD_DOCUMENTATION:BOOL', self.controlData.mDoxygen)            
    
    def _assembleOptions(self):
        return " ".join(["-D%s=%s"%(key,val) for key,val in self.cmakeOptions.iteritems()])
    def _printOptions(self):
        options = "".join(["    %s = %s\n"%(key,val) for key,val in self.cmakeOptions.iteritems()])
        print "*** CMake Options:\n", options

    def _changeDirToBase(self):
        changeDir(self.mBasePath)
    def _changeDirToSource(self):
        changeDir(self.mSourcePath)
    def _changeDirToBuild(self):
        changeDir(self.mBuildPath)
# ---------------------------------------------------------
