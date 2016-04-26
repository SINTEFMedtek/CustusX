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
        shell.removeTree(self.mBuildPath)
            
    def build(self):
        self._changeDirToBuild()
        #self._changeDirToBuild()
        if(platform.system() == 'Windows'):
            if(self.controlData.getCMakeGenerator() == 'Eclipse CDT4 - NMake Makefiles'):
                runShell('nmake')
            if(self.controlData.getCMakeGenerator() == 'NMake Makefiles JOM'):
                runShell('''jom -k -j%s''' % str(self.controlData.threads))
            if(self.controlData.getCMakeGenerator() == 'Eclipse CDT4 - Ninja'):
                runShell('''ninja''')
        else:
            maker = 'make -j%s' % str(self.controlData.threads)
            if(self.controlData.getCMakeGenerator() == 'Eclipse CDT4 - Ninja'):
                maker = 'ninja'

            # the export DYLD... line is a hack to get shared linking to work on MacOS with vtk5.6
            # - http://www.mail-archive.com/paraview@paraview.org/msg07520.html
            # (add it to all project because it does no harm if not needed)
            runShell('''\
    export DYLD_LIBRARY_PATH=`pwd`/bin; \
    %s
    ''' % maker)

    def gitClone(self, repository, folder=''):
        self._changeDirToBase()
        runShell('git clone %s %s' % (repository, folder))
        self._changeDirToSource()
        
    def gitCloneIntoExistingDirectory(self, repository, branch):
        '''
        Use in the case that the source folder already contains stuff,
        http://stackoverflow.com/questions/2411031/how-do-i-clone-into-a-non-empty-directory
        '''
        self._changeDirToSource()    
        runShell('git init')
        runShell('git remote add origin %s' % repository)
        runShell('git fetch')
        runShell('git checkout -t origin/%s' % branch)        

    def gitSetRemoteURL(self, new_remote_origin_repository, branch=None):
        self._changeDirToSource()
        runShell('git remote set-url origin %s' % new_remote_origin_repository)
        runShell('git fetch')
        # old (1.7) syntax - update if needed to 'git branch --set-upstream-to origin/<branch>' 
        if branch!=None:
            runShell('git branch --set-upstream %s origin/%s' % (branch, branch), ignoreFailure=True) # can fail if branch does not exist, might happen if a nonstandard branch is selected.
        #runShell('git branch -u origin/%s' % branch)

    def _gitSubmoduleUpdate(self):
        self._changeDirToSource()
        runShell('git submodule sync') # from tsf 
        runShell('git submodule update --init --recursive')                    

    def gitCheckoutDefaultBranch(self, submodules=False):
        '''
        checkout the default branch as set by default or user.
        '''
        tag = self.controlData.getGitTag()
        branch = self.controlData.main_branch
        if (tag==None) or (tag=="\"\""):
            self.gitCheckoutBranch(branch, submodules)
        else:
            self.gitCheckoutTag(tag, submodules=submodules)

    def gitCheckoutBranch(self, branch, submodules=False):
        '''
        pull latest version of branch, include submodules if asked.
        '''
        self._changeDirToSource()
        #if branch contains only '' set as empty .... todo

        runShell('git fetch')
	# If the branch doesn't exist, this might be ok: 
	# only a subset of the repos need have the branch defined.
	# In this case, checkout the latest stuff on the master branch. - changed - see discussion below.
        if runShell('git checkout %s' % branch, ignoreFailure=True):
                runShell('git pull origin %s' % branch, ignoreFailure=True)
        else:
                # This fallback is becoming problematic: if input branch is fex feature/something, and this
                # undefined in some repos, we would like to fallback to the develop branch, IF using git flow,
                # but if working on a release, we want master. 
                # Best guess: develop.
                print "* Warning: checkout branch %s failed: guessing at branch develop as fallback:" % branch
                runShell('git checkout develop', ignoreFailure=True)
                runShell('git pull origin develop', ignoreFailure=True)
        if submodules:
            self._gitSubmoduleUpdate()

    def gitCheckoutTag(self, tag, submodules=False):
        '''
        Update git to the given tag.
        Skip if HEAD already is at tag.
        '''
        self._changeDirToSource()
        if self._checkGitIsAtTag(tag):
            return        
        runShell('git fetch')
        if not runShell('git checkout %s' % tag):
            # fallback: this is a guess only
            print "* Warning: checkout tag %s failed: guessing at branch develop as fallback:" % tag
            runShell('git checkout develop')
        if submodules:
            self._gitSubmoduleUpdate()
        
    def gitCheckout(self, tag, submodules=False):
        '''
        Backwards compatibility
        '''
        self.gitCheckoutTag(tag, submodules)

    def _checkGitIsAtTag(self, tag):
        output = shell.evaluate('git describe --tags --exact-match')
        if not output:
            return False
        if output.stdout.strip()==tag:
            print "Skipping git update: Tag %s already at HEAD in %s" % (tag, self.mSourcePath)
            return True
        return False
                    
    def _getPathToModule(self):
        # alternatively use  sys.argv[0] ?? 
        moduleFile = os.path.realpath(__file__)
        modulePath = os.path.dirname(moduleFile)
        modulePath = os.path.abspath(modulePath)
        return modulePath
                 
    def makeClean(self):
        self._changeDirToBuild()
        #self._changeDirToBuild()
        if(self.controlData.getCMakeGenerator() == 'Eclipse CDT4 - Ninja'):
            runShell('ninja clean')
        elif(platform.system() == 'Windows'):
            if(self.controlData.getCMakeGenerator() == 'Eclipse CDT4 - NMake Makefiles'):
                runShell('nmake -clean')
            if(self.controlData.getCMakeGenerator() == 'NMake Makefiles JOM'):
                runShell('jom -clean')
        else:
            runShell('make clean')

    def addCMakeOption(self, key, value):
        self.cmakeOptions[key] = value

    def configureCMake(self, options=''): 
        self._addDefaultCmakeOptions()
        generator = self.controlData.getCMakeGenerator()
        optionsFromAssembly = self._assembleOptions()
        self._printOptions()        
        cmd = 'cmake -G"%s" %s %s %s'
        cmd = cmd % (generator, options, optionsFromAssembly, self.mSourcePath)        

        self._changeDirToBuild()
        runShell(cmd)

    def _addDefaultCmakeOptions(self):
        add = self.addCMakeOption
        if(platform.system() != 'Windows'):
            add('CMAKE_CXX_FLAGS:STRING', '-Wno-deprecated')
        add('CMAKE_BUILD_TYPE:STRING', self.mBuildType)        
        if self.controlData.m32bit: # todo: add if darwin
            add('CMAKE_OSX_ARCHITECTURES', 'i386')
        if('BUILD_SHARED_LIBS:BOOL' not in self.cmakeOptions):
            add('BUILD_SHARED_LIBS:BOOL', self.controlData.getBuildShared())
        add('CMAKE_ECLIPSE_VERSION', self.controlData.getEclipseVersion())
        add('CMAKE_ECLIPSE_GENERATE_LINKED_RESOURCES', False)            
    
    def _assembleOptions(self):
        return " ".join(['-D%s="%s"'%(key,val) for key,val in self.cmakeOptions.iteritems()])
    
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
