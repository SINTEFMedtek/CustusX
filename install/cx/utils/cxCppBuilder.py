#!/usr/bin/env python3

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#
# Description:
#
#             
#################################################             

from __future__ import print_function
from __future__ import absolute_import
import subprocess
import optparse
import re
import sys
import os.path
import time
import urllib.request, urllib.parse, urllib.error
import getpass
import platform
import shutil
from . import cxRepoHandler

from .cxShell import *
    
class CppBuilder(object):
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
            if(self.controlData.getCMakeGenerator() == 'Ninja'):
                runShell('''ninja''')
        else:
            if(self.controlData.getCMakeGenerator() == 'Ninja'):
                maker = 'ninja -j%s' % str(self.controlData.threads)
            else:
                maker = 'make -j%s' % str(self.controlData.threads)

            # the export DYLD... line is a hack to get shared linking to work on MacOS with vtk5.6
            # - http://www.mail-archive.com/paraview@paraview.org/msg07520.html
            # (add it to all project because it does no harm if not needed)
            runShell('''\
    export DYLD_LIBRARY_PATH=`pwd`/bin; \
    %s
    ''' % maker)

    def gitClone(self, repository, folder=''):
        self._changeDirToBase()
        self._gitCloneWithRetry(repository, folder)
        self._changeDirToSource()

    def gitCloneAtTag(self, repository, tag, folder=''):
        '''
        Like gitClone(), but shallow: clones directly at a known, pinned tag
        (--branch <tag> --depth 1) instead of full history (CustusX#50). For
        a large, chronically-flaky-to-clone repo whose tag is fixed and known
        ahead of time (VTK/VTK92 in particular -- several GB of history vs a
        few hundred MB shallow), this is both faster and far less likely to
        drop mid-transfer. Verified directly against gitlab.kitware.com/vtk/
        vtk.git: a --depth 1 clone at a pinned tag leaves `git describe --tags
        --exact-match` (what isAtTag()/update() rely on) resolving correctly,
        so the caller's usual isAtTag() check finds it already at the tag and
        skips its own update() work, same as any other build.
        '''
        self._changeDirToBase()
        self._gitCloneAtTagWithRetry(repository, tag, folder)
        self._changeDirToSource()

    def _gitCloneWithRetry(self, repository, folder, attempts=3):
        '''
        A bare `git clone` has no resilience against a mid-transfer TLS drop
        (e.g. GnuTLS recv error). Retry here, falling back to HTTP/1.1 since
        HTTP/2 is the more likely side to drop the connection on some
        networks/proxies. Mirrors cxRepoHandler.RepoHandler._cloneWithRetry.
        '''
        target = os.path.join(self.mBasePath, folder) if folder else self.mBasePath
        for attempt in range(1, attempts + 1):
            http_fallback = '' if attempt == 1 else '-c http.version=HTTP/1.1 '
            cmd = 'git %sclone %s %s' % (http_fallback, repository, folder)
            if runShell(cmd, ignoreFailure=True):
                return
            print('Clone attempt %d/%d failed for %s.' % (attempt, attempts, repository))
            if os.path.exists(target):
                shutil.rmtree(target)
        exit('ERROR: failed to clone %s after %d attempts.' % (repository, attempts))

    def _gitCloneAtTagWithRetry(self, repository, tag, folder, attempts=3):
        '''
        Mirrors _gitCloneWithRetry, shallow at a known tag instead of full
        history -- see gitCloneAtTag() above.
        '''
        target = os.path.join(self.mBasePath, folder) if folder else self.mBasePath
        for attempt in range(1, attempts + 1):
            http_fallback = '' if attempt == 1 else '-c http.version=HTTP/1.1 '
            cmd = 'git %sclone --branch %s --depth 1 %s %s' % (http_fallback, tag, repository, folder)
            if runShell(cmd, ignoreFailure=True):
                return
            print('Shallow clone attempt %d/%d failed for %s@%s.' % (attempt, attempts, repository, tag))
            if os.path.exists(target):
                shutil.rmtree(target)
            if attempt < attempts:
                time.sleep(5)
        exit('ERROR: failed to shallow-clone %s@%s after %d attempts.' % (repository, tag, attempts))

    def _gitFetchWithRetry(self, attempts=3):
        '''
        A bare `git fetch` has no resilience against a transient network
        failure (e.g. a connection timeout to the remote), unlike
        _gitCloneWithRetry above. Every checkout/update path below fetches
        unconditionally on each build, so a single flaky remote (e.g.
        gitlab.kitware.com being briefly unreachable) would otherwise call
        exit() and kill the whole build. Retry here too, falling back to
        HTTP/1.1 since HTTP/2 is the more likely side to drop the connection
        on some networks/proxies. Mirrors _gitCloneWithRetry/
        cxRepoHandler.RepoHandler._cloneWithRetry.
        '''
        for attempt in range(1, attempts + 1):
            http_fallback = '' if attempt == 1 else '-c http.version=HTTP/1.1 '
            cmd = 'git %sfetch' % http_fallback
            if runShell(cmd, ignoreFailure=True):
                return
            print('Fetch attempt %d/%d failed.' % (attempt, attempts))
            if attempt < attempts:
                # A transient network blip (e.g. gitlab.kitware.com briefly
                # unreachable) needs a moment to clear -- retrying 3x back to
                # back with no delay (as before) rarely gives it enough time,
                # and this fires unconditionally for every component's
                # update(), unlike _gitCloneWithRetry which only runs once
                # per fresh checkout. Matches the install scripts' own
                # download_with_retry() 5s pause for the same class of issue.
                time.sleep(5)
        exit('ERROR: failed to fetch after %d attempts.' % attempts)

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

    def gitSetRemoteURL(self, new_remote_origin_repository, branch=None, fetch=True):
        '''
        `fetch=False` lets a component's update() still self-heal the remote
        URL (a cheap, local git-config operation) every time, per CLAUDE.md's
        documented invariant, while skipping only the network fetch that
        follows it -- e.g. when isAtTag() already confirmed there's nothing
        new to fetch for a pinned dependency (CustusX#46/CustusX#50).
        '''
        self._changeDirToSource()
        runShell('git remote set-url origin %s' % new_remote_origin_repository)
        if fetch:
            self._gitFetchWithRetry()
        # old (1.7) syntax - update if needed to 'git branch --set-upstream-to origin/<branch>'
        if branch!=None:
            runShell('git branch --set-upstream %s origin/%s' % (branch, branch), ignoreFailure=True) # can fail if branch does not exist, might happen if a nonstandard branch is selected.
        #runShell('git branch -u origin/%s' % branch)

    def _gitSubmoduleUpdate(self):
        self._changeDirToSource()
        runShell('git submodule sync') # from tsf 
        runShell('git submodule update --init --recursive')                    

    def gitCheckoutDefaultBranch(self):
        '''
        checkout the default branch as set by default or user.
        '''
        repo = cxRepoHandler.RepoHandler()
        repo.setRepoInfo(root_path=self.mBasePath, repo_path=self.mSourcePath)
        repo.setBranchDefault(self.controlData.main_branch)
        repo.syncToGitRef()

    def gitCheckoutBranch(self, branch, submodules=False):
        '''
        pull latest version of branch, include submodules if asked.
        '''
        self._changeDirToSource()
        self._gitFetchWithRetry()
        runShell('git checkout %s' % branch)
        runShell('git pull origin %s' % branch)


    def gitCheckoutTag(self, tag):
        '''
        Update git to the given tag or SHA.
        Skip if HEAD already is at tag/SHA, but warn if local modifications exist.
        '''
        self._changeDirToSource()
        if self._checkGitIsAtTag(tag):
            return
        self._gitFetchWithRetry()
        if self._checkGitIsAtRef(tag):
            self._warnIfLocalModifications(tag)
            return
        runShell('git checkout %s' % tag)

    def gitCheckout(self, tag):
        '''
        Backwards compatibility
        '''
        self.gitCheckoutTag(tag)

    def gitCheckoutSha(self, sha):
        '''
        Use this function when checking out a git sha,
        instead of gitCheckout/gitCheckoutTag,
        as they will output confusing warnings
        '''
        self._changeDirToSource()
        self._gitFetchWithRetry()
        if self._checkGitIsAtRef(sha):
            self._warnIfLocalModifications(sha)
            return
        runShell('git checkout %s' % sha)

    def isAtTag(self, tag):
        '''
        True if the source repo's HEAD is already exactly at the given tag.
        Lets a component's update() skip the network fetch inside
        gitSetRemoteURL() (pass fetch=False there) for a pinned external
        dependency whose tag rarely or never changes between builds (e.g.
        VTK) - avoiding a round-trip to a remote that may be temporarily or
        permanently unreachable even though nothing here needs to change
        (CustusX#46/CustusX#50). gitSetRemoteURL() itself must still run
        unconditionally -- only the fetch inside it is safe to skip.
        '''
        self._changeDirToSource()
        return self._checkGitIsAtTag(tag)

    def _checkGitIsAtTag(self, tag):
        output = shell.evaluate('git describe --tags --exact-match')
        if not output:
            return False
        if output.stdout.strip()==tag:
            print("Skipping git update: Tag %s already at HEAD in %s" % (tag, self.mSourcePath))
            return True
        return False

    def _checkGitIsAtRef(self, ref):
        'Return True if HEAD resolves to the same commit as ref'
        head = shell.evaluate('git rev-parse HEAD')
        target = shell.evaluate('git rev-parse %s^{}' % ref)
        if head and target and head.stdout.strip() == target.stdout.strip():
            return True
        return False

    def _warnIfLocalModifications(self, ref):
        result = shell.evaluate('git diff --name-only HEAD')
        if not result or not result.stdout.strip():
            return
        print('WARNING: Already at %s in %s, but the following files have local modifications:' % (ref, self.mSourcePath))
        print(result.stdout.strip())
        print('These may affect the build. Run "git reset --hard HEAD" in that folder to restore committed state.')
                    
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
        if('CMAKE_CXX_FLAGS:STRING' == key):
            print('WARNING: CMAKE_CXX_FLAGS must be added by CMakeFiles, skipping: '+value)
            return
        self.cmakeOptions[key] = value

    def appendCMakeOption(self, key, value):
        temp = ""
        if(key in self.cmakeOptions):
            temp = self.cmakeOptions[key]
            print(key+" was set to "+temp)
        if(not temp):
            new = value
        else:
            new = temp+" "+value
        print(key+" is now set to "+new)
        self.cmakeOptions[key] = new

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
        append = self.appendCMakeOption
        if(platform.system() != 'Windows'):
            # append('CX_CMAKE_CXX_FLAGS:STRING', '-Wno-deprecated -Wno-unknown-warning-option -Wno-inconsistent-missing-override -Wno-self-assign-field')
            append('CX_CMAKE_CXX_FLAGS:STRING', '-Wno-deprecated')
        if(platform.system() == 'Windows' and self.controlData.getCMakeGenerator() == 'Ninja'):
            # Without this, Ninja builds compile commands as a single CreateProcess call.
            # The superbuild's VTK/ITK/CTK include-path lists routinely exceed Windows'
            # 32767-char command-line limit, causing "CreateProcess failed... is the
            # command line too long?" errors on targets like cxResourceVisualization.
            # Forcing response files makes cl.exe read args from an @file instead.
            add('CMAKE_NINJA_FORCE_RESPONSE_FILE:BOOL', 'ON')
        add('CMAKE_BUILD_TYPE:STRING', self.mBuildType)
        if self.controlData.m32bit: # todo: add if darwin
            add('CMAKE_OSX_ARCHITECTURES', 'i386')
        if(platform.system() == 'Darwin' and platform.machine() == 'arm64'):
            if('CMAKE_OSX_ARCHITECTURES' not in self.cmakeOptions and 'CMAKE_OSX_ARCHITECTURES:STRING' not in self.cmakeOptions):
                add('CMAKE_OSX_ARCHITECTURES:STRING', 'arm64')
            if('CMAKE_PREFIX_PATH:PATH' not in self.cmakeOptions):
                add('CMAKE_PREFIX_PATH:PATH', '/opt/homebrew')
        if('BUILD_SHARED_LIBS:BOOL' not in self.cmakeOptions):
            add('BUILD_SHARED_LIBS:BOOL', self.controlData.getBuildShared())
        add('CMAKE_ECLIPSE_VERSION', self.controlData.getEclipseVersion())
        add('CMAKE_ECLIPSE_GENERATE_LINKED_RESOURCES', False)
    
    def _assembleOptions(self):
        return " ".join(['-D%s="%s"'%(key,val) for key,val in self.cmakeOptions.items()])
    
    def _printOptions(self):
        options = "".join(["    %s = %s\n"%(key,val) for key,val in self.cmakeOptions.items()])
        print("*** CMake Options:\n", options)

    def _changeDirToBase(self):
        changeDir(self.mBasePath)
    
    def _changeDirToSource(self):
        changeDir(self.mSourcePath)
    
    def _changeDirToBuild(self):
        changeDir(self.mBuildPath)
# ---------------------------------------------------------
