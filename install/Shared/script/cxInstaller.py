#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#
# Description:
#
# Script for installation of new workstation for CustusX
# based on description from:
# http://cxserver.sintef.no:16080/wiki/index.php/Installeringsveiviser
#
# Run cxInstaller --help to get usage info.
#
# prerequisites:
# boost, cmake, cppunit, eclipse
# Need a github key installed.
#
# Default values:
# - change the class Common
#
# default folder setup (feel free to modify)
#
# --root---external---ITK---ITK
#    |         |       |----build_Debug  
#    |         |       |----build_Release
#    |         |       |----build32_Debug
#    |         |
#    |         |------VTK---VTK
#    |                 |----build_Debug  
#    |                 |----build_Release
#    |                 |----build32_Debug
#    |-----workspace
#              |------CustusX---CustusX
#                      |--------build_Debug  
#                      |--------build_Release
#                      |--------build32_Debug
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

class Common(object):
    '''
    Container for common data.
    Modify these to change behaviour
    '''
    def __init__(self):
        self.PLATFORM = platform.system() # {Windows7 = 'Windows', OSX 10.7.4 = 'Darwin', Liux = ?}
        self.mUser = getpass.getuser()
        self.mISBpassword = ""
        if(self.PLATFORM == 'Windows'):
            self.mRootDir = "C:\Dev"
        else:
            self.mRootDir = os.path.expanduser("~") + "/" + self.mUser
        # external dir: Used as base dir for all externals, such as VTK, ITK, ...
        self.mExternalDir = self.mRootDir + "/external_code"
        # working dir: Used as base dir for Custus and other of our 'own' projects
        self.mWorkingDir = self.mRootDir + "/workspace"
        # server user: Used for login to cx server etc.
        self.mServerUser = self.mUser
        self.mSilent_mode = False
        # build as shared or static libraries
        self.mBuildShared = "ON" # Change to ON or OFF
        self.mBuildType = "Debug" # Debug, Release, RelWithDebInfo, MinSizeRel
        self.mBuildExternalsType = "Release" # used for all non-cx libs, this because we want speed even in debug...
        self.mBuildFolder = "build" # default build folder. This is auto-changed when using xcode or 32 bit.
        self.m32bitCompileCMakeOption = "" # use "-DCMAKE_OSX_ARCHITECTURES=i386" for 32 bit. Done automatically by settings --b32 from command line.
        self.mBuildSSCExamples = "ON"
        self.mBuildTesting = "ON"
        self.mUseCotire = "OFF"
        self.mSerialPort = "/Library/CustusX/igstk.links/cu.CustusX.dev0"
        self.mOpenCVStaticCRT = "OFF"
        self.mOpenCVWithJasper = "OFF"
        self.mOpenCVWithTiff = "OFF"
        self.mOSX_DEPLOYMENT_TARGET = "10.6" # Deploy for OSX 10.6 Snow Leopard and later
        if (self.PLATFORM == 'Windows'):
            self.mCMakeGenerator = 'Eclipse CDT4 - NMake Makefiles' # need to surround with ' ' instead of " " on windows for it to work
            self.mBuildSSCExamples = "OFF"
            self.mBuildTesting = "OFF"
            self.mExternalDir = self.mRootDir + "/external" #path length on windows is limited, need to keep it short
            self.mUseCotire = "ON"
            self.mSerialPort = "COM9"
            self.mOpenCVStaticCRT = "OFF"
        else:
            self.mCMakeGenerator = "Eclipse CDT4 - Unix Makefiles" # or "Xcode". Use -eclipse or -xcode from command line. Applies only to workspace projects.
        self.mBuildExAndTest = "OFF"
        self.mCoverage = "OFF"
        self.mCMakeArgs = ""


# ---------------------------------------------------------
    
class Shell (object):
    '''
    Superclass for platform specific shells like:
    -cmd (Windows)
    -bash (Mac & Linux)
    '''
    def __init__(self):
        self.DUMMY = False
        self.VERBOSE = False
        if(DATA.PLATFORM == 'Windows'):
            self.CWD = "C:\\"
        else:
            self.CWD = '/' # remember directory
        self.password = ""
        
    def _runReal(self, cmd):
        '''This function runs shell, no return, insert password'''
        print '*** run:', cmd
        p = subprocess.Popen(cmd, shell=True, cwd=self.CWD)
        p.communicate("") # seems to be needed in order to wait for password prompting...?
    
    def _runDummy(self, cmd):
        '''Dummy version of shell'''
        if cmd=="whoami":
            #return self._runReal(cmd) #does not work as expected on windows
            return DATA.mUser
        print "*** dummy run:", cmd
        return ""
    
    def run(self, cmd):
        '''Run a shell script'''
        if self.DUMMY is True:
            return self._runDummy(cmd)
        else:
            return self._runReal(cmd)
           
    def evaluate(self, cmd):
        '''This function takes shell commands and returns them'''
        # print '*** eval run:', cmd
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=self.CWD)
        out = p.stdout.read().strip()
        return out #This is the stdout from the shell command
    
    def changeDir(self, path):
        self.CWD = path
    
# ---------------------------------------------------------
    
class Cmd (Shell):
    '''
    Interface to the Windows command shell (cmd.exe).
    '''
    def __init__(self):
        super(Cmd, self).__init__()
    
    def changeDir(self, path):
        path = path.replace("/", "\\")
        Shell.run(self, 'cmd /C mkdir '+path)
        Shell.changeDir(self, path)

# ---------------------------------------------------------

class Bash (Shell):
    '''
    Wrapper for methods that call the bash shell.
    Also keeps track of the current directory through changeDir()
    '''
    def __init__(self):
        super(Bash, self).__init__()
    
    def changeDir(self, path):
        '''mkdir + cd bash operations'''
        path = path.replace("\\", "/")
        Shell.run(self, 'mkdir -p '+path)
        Shell.changeDir(self, path)

# --------------------------------------------------------

# global variable for data
DATA = Common()
# global for shell interaction
shell = Shell()
if(DATA.PLATFORM == 'Windows'):
    shell = Cmd()
else:
    shell = Bash() 
# shortcuts
runShell = shell.run
changeDir = shell.changeDir

# ---------------------------------------------------------

class Component(object):
    '''
    Data for one Component
    Superclass for all specific components,
    which must implement this interface.
    '''
    def __init__(self):
        pass
    def name(self):
        'return name of component'
        raise "Not Implemented"
    def help(self):
        'description of item'
        return ''
    def path(self):
        'return path where component will be installed'
        raise "Not Implemented"
    def checkout(self):
        'checkout the component source from external source to this computer (svn co or similar)'
        path = self.path()+'/'+self.sourceFolder()
        if os.path.exists(path) and len(os.listdir(path))!=0:
            print "*** %s already exists, checkout ignored." % self.name()
        else:
            self._rawCheckout()
    def _rawCheckout(self):
        'checkout the component source from external source to this computer (svn co or similar)'
        raise "Not Implemented"
    def update(self):
        'update the component source (svn up or similar)'
        pass
    def configure(self):
        'configure build of components (cmake or similar)'
        pass
    def reset(self):
        'delete build folder(s)'
        pass
    def build(self):
        'build and install component (make and possibly make install)'
        pass
    def makeClean(self):
        'make clean'
        pass
    def buildConfigurations(self):
        return []
        
# ---------------------------------------------------------

class CppComponent(Component):
    '''
    Implementation of Cpp-style components, i.e all c++ libraries
    that contains a source and build folder, and are built with make.
    Inherit from this to get some methods for free.
    '''
    def __init__(self):
        pass
    def sourceFolder(self):
        return self.name()
    def buildFolder(self):
        return DATA.mBuildFolder
    def _changeDirTo(self, folder):
        changeDir(self.path()+'/'+folder)
    def _changeDirToBase(self):
        changeDir(self.path())
    def _changeDirToSource(self):
        changeDir(self.path()+'/'+self.sourceFolder())
    def _changeDirToBuild(self):
        changeDir(self.buildPath())
    def buildPath(self):
        return self.path()+'/'+self.buildFolder()
    def installPath(self):
        return self.buildPath()
    def _rawCheckout(self):
        raise "Not Implemented"
    def configure(self):
        raise "Not Implemented"
    def reset(self):
        'delete build folder(s)'
        self._changeDirToBase()
        if(DATA.PLATFORM == 'Windows'):
            #runShell('echo WANT TO REMOVE FOLDER %s/%s' % (self.path(), self.buildFolder()))
            runShell('rd /S /Q "%s/%s"' % (self.path(), self.buildFolder()))
        else:
            runShell('rm -R -f %s/%s' % (self.path(), self.buildFolder()))
    def build(self):
        self._changeDirToBuild()
        if(DATA.PLATFORM == 'Windows'):
            if(DATA.mCMakeGenerator == 'Eclipse CDT4 - NMake Makefiles'):
                runShell('nmake')
            if(DATA.mCMakeGenerator == 'NMake Makefiles JOM'):
                runShell('''jom -k -j%s''' % str(DATA.options.makethreads))
        else:
            # the export DYLD... line is a hack to get shared linking to work on MacOS with vtk5.6
            # - http://www.mail-archive.com/paraview@paraview.org/msg07520.html
            # (add it to all project because it does no harm if not needed)
            runShell('''\
    export DYLD_LIBRARY_PATH=`pwd`/bin; \
    make -j%s
    ''' % str(DATA.options.makethreads))
    def makeClean(self):
        self._changeDirToBuild()
        if(DATA.PLATFORM == 'Windows'):
            if(DATA.mCMakeGenerator == 'Eclipse CDT4 - NMake Makefiles'):
                runShell('nmake -clean')
            if(DATA.mCMakeGenerator == 'NMake Makefiles JOM'):
                runShell('jom -clean')
        else:
            runShell('make clean')
# ---------------------------------------------------------

#===============================================================================
# class CppUnit(CppComponent):
#    def name(self):
#        return "CppUnit"
#    def help(self):
#        return "http://sourceforge.net/projects/cppunit/"
#    def path(self):
#        return DATA.mExternalDir + "CppUnit"
#    def _rawCheckout(self):
#        self._changeDirToBase()
#        runShell('')
#===============================================================================
# ---------------------------------------------------------

class ITK(CppComponent):
    def name(self):
        return "ITK"
    def help(self):
        return 'itk.org'
    def path(self):
        return DATA.mExternalDir + "/ITK"
    def sourceFolder(self):
        'override default source folder name'
        return 'ITK'
    def _rawCheckout(self):
        self._changeDirToBase()
        runShell('git clone git://itk.org/ITK.git')
        self.update()
    def update(self):
        self._changeDirToSource()
        #runShell('git checkout master') # not needed
        #runShell('git pull')
            #runShell('git checkout v3.20.0') # version working ok with IGSTK 4.2
        runShell('git checkout v4.1.0')
    def configure(self):
        self._changeDirToBuild()
        runShell('''\
cmake \
-G"%s" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DBUILD_TESTING=%s \
-DBUILD_EXAMPLES=%s \
-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
../%s''' % (DATA.mCMakeGenerator,
            DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildShared, 
            DATA.mBuildExAndTest, 
            DATA.mBuildExAndTest, 
            DATA.mOSX_DEPLOYMENT_TARGET,
            self.sourceFolder()))
# ---------------------------------------------------------

class VTK(CppComponent):
    def name(self):
        return "VTK"
    def help(self):
        return 'vtk.org'
    def path(self):
        return DATA.mExternalDir + "/VTK"
    def _rawCheckout(self):
        self._changeDirToBase()
        runShell('git clone http://vtk.org/VTK.git')
        self.update()
    def update(self):
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
		#Note: vtk 5.10 contains a bug in STLReader. Skip that version (http://vtk.1045678.n5.nabble.com/VTK-0013160-vtkSTLReader-does-not-read-files-any-more-td5700473.html)
        runShell('git checkout v5.8.0') 
        runShell('git branch -D cx_mod_for_5-8-0')
        runShell('git checkout -B cx_mod_for_5-8-0')
        #TODO this can be a bug, if CustusX is not checked out yet, this will not work!!!
        runShell(('git am --whitespace=fix --signoff < %s/%s/install/Shared/script/VTK-5-8-0.patch') % (CustusX3().path(), CustusX3().sourceFolder()))
    def configure(self):
        '''
Note: DVTK_REQUIRED_OBJCXX_FLAGS is required on v5.6 in order to avoid garbage-collection (!)
'''
        self._changeDirToBuild()
        runShell('''\
cmake \
-G"%s" \
%s \
-DCMAKE_CXX_FLAGS:STRING=-Wno-deprecated \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DVTK_USE_PARALLEL:BOOL=ON \
-DVTK_REQUIRED_OBJCXX_FLAGS:STRING="" \
-DBUILD_EXAMPLES:BOOL=%s \
-DBUILD_TESTING:BOOL=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DVTK_USE_GUISUPPORT:BOOL=ON \
-DVTK_USE_QT:BOOL=ON \
-DVTK_USE_QVTK:BOOL=ON \
-DVTK_USE_RPATH:BOOL=ON \
-DDESIRED_QT_VERSION:STRING=4 \
-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
../%s''' % (DATA.mCMakeGenerator,
            DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildExAndTest,
            DATA.mBuildExAndTest,
            DATA.mBuildShared, 
            DATA.mOSX_DEPLOYMENT_TARGET,
            self.sourceFolder()))
# ---------------------------------------------------------

class OpenCV(CppComponent):
    def name(self):
        return "OpenCV"
    def help(self):
        return 'http://opencv.willowgarage.com'
#    def sourceFolder(self):
#        return self.name()+"/opencv"
    def path(self):
        return DATA.mExternalDir + "/OpenCV"
    def _rawCheckout(self):
        self._changeDirToBase()
#        runShell('svn co https://code.ros.org/svn/opencv/trunk/opencv OpenCV')
#        runShell('svn co https://code.ros.org/svn/opencv/branches/2.3/opencv OpenCV') #old location
        #runShell('svn co http://code.opencv.org/svn/opencv/branches/2.3/opencv OpenCV')
        #runShell('svn co http://code.opencv.org/svn/opencv/branches/2.4/opencv OpenCV') #new version for lion compatibility
        runShell('git clone git://code.opencv.org/opencv.git OpenCV') #OpenCV moved to git, no longer available on svn
    def update(self):
        self._changeDirToSource()
        runShell('git pull')
        runShell('git checkout 2.4.2')
    def configure(self):
        self._changeDirToBuild()
        runShell('''\
cmake \
-G"%s" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_EXAMPLES:BOOL=%s \
-DBUILD_TESTS:BOOL=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DWITH_CUDA:BOOL=OFF \
-DBUILD_WITH_STATIC_CRT:BOOL=%s \
-DBUILD_TIFF:BOOL=%s \
-DWITH_TIFF:BOOL=%s \
-DBUILD_JASPER:BOOL=%s \
-DWITH_JASPER:BOOL=%s \
-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
../%s''' % (DATA.mCMakeGenerator,
            DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildExAndTest,
            DATA.mBuildExAndTest,
            DATA.mBuildShared,
            DATA.mOpenCVStaticCRT,
            DATA.mOpenCVWithTiff,
            DATA.mOpenCVWithTiff,
            DATA.mOpenCVWithJasper,
            DATA.mOpenCVWithJasper,
            DATA.mOSX_DEPLOYMENT_TARGET,
            self.sourceFolder() ))
# ---------------------------------------------------------


class OpenIGTLink(CppComponent):
    def name(self):
        return "OpenIGTLink"
    def help(self):
        return 'http://www.na-mic.org/Wiki/index.php/OpenIGTLink/Library/Build'
    def path(self):
        return DATA.mExternalDir + "/OpenIGTLink"
    def _rawCheckout(self):
        self._changeDirToBase()
        #runShell('svn co http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink OpenIGTLink')
        runShell('git clone git://github.com/openigtlink/OpenIGTLink.git')
    def update(self):
        self._changeDirToSource()
        runShell('git checkout master')
    def configure(self):
        self._changeDirToBuild()
        runShell('\
cmake \
-G"%s" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_EXAMPLES:BOOL=%s \
-DBUILD_TESTING:BOOL=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
../%s' % (DATA.mCMakeGenerator,
            DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildExAndTest,
            DATA.mBuildExAndTest,
            DATA.mBuildShared, 
            DATA.mOSX_DEPLOYMENT_TARGET,
            self.sourceFolder() ))
# ---------------------------------------------------------


class IGSTK(CppComponent):
    def name(self):
        return "IGSTK"
    def help(self):
        return 'igstk.org'
    def path(self):
        return DATA.mExternalDir + "/IGSTK"
    def _rawCheckout(self):
        self._changeDirToBase()
        runShell('git clone git://igstk.org/IGSTK.git') #from v5.0 igstk is now available using git
        #runShell('''\
#cvs -d :pserver:anonymous:igstk@public.kitware.com:/cvsroot/IGSTK login
#cvs -d :pserver:anonymous@public.kitware.com:/cvsroot/IGSTK checkout -r "IGSTK-4-4" IGSTK
#cvs -d :pserver:anonymous@public.kitware.com:/cvsroot/IGSTK logout
#''')
    def update(self):
        self._changeDirToSource()
        runShell('git checkout v5.0')
        runShell('git branch -D cx_mod_for_50')
        runShell('git checkout -B cx_mod_for_50')
        #TODO this can be a bug, if CustusX is not checked out yet, this will not work!!!
        runShell(('git am --whitespace=fix --signoff < %s/%s/install/Shared/script/IGSTK-5-0-v2.patch') % (CustusX3().path(), CustusX3().sourceFolder()))
    def configure(self):
        self._changeDirToBuild()
        runShell('''\
cmake \
-G"%s" \
%s \
-DCMAKE_CXX_FLAGS:STRING=-Wno-deprecated \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DIGSTK_USE_SceneGraphVisualization:BOOL=OFF \
-DBUILD_EXAMPLES:BOOL=OFF \
-DBUILD_TESTING:BOOL=OFF \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DBUILD_TESTING:BOOL=OFF \
-DITK_DIR:PATH="%s" \
-DVTK_DIR:PATH="%s" \
-DIGSTK_SERIAL_PORT_0="%s" \
-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
../%s''' % (DATA.mCMakeGenerator,
            DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildShared, 
            ITK().buildPath(), 
            VTK().buildPath(), 
            DATA.mSerialPort,
            DATA.mOSX_DEPLOYMENT_TARGET,
            self.sourceFolder())
            )
# ---------------------------------------------------------

#===============================================================================
# class DCMTK(CppComponent):
#    '''
#    The only way to get 3.6.1 to work is to install it,
#    thats the reason for the special treatment in this class.
#    
#    This might change in the future.
#    '''
#    def name(self):
#        return "DCMTK"
#    #def buildFolder(self):
#        #'in-source build because this is necessary for including an uninstalled DCMTK'
#        #return self.sourceFolder()    
#    def help(self):
#        return 'dcmtk.org'
#    def path(self):
#        return DATA.mExternalDir + "/DCMTK"
# 
#    def _rawCheckout(self):
#        self._changeDirToBase()
#        runShell('git clone git://git.dcmtk.org/dcmtk DCMTK')
#        # the commontk version of DCMTK compiles without problems on Mac.
#        #runShell('git clone git://github.com/commontk/DCMTK.git DCMTK')     
#        self.update()
#    def update(self):
#        self._changeDirToSource()
#        #runShell('git pull')
#        #runShell('git checkout master')   
#        runShell('git checkout PUBLIC_360')  # 3.6.0 seems to have some issues on fedora 16.  
# 
#    def configure(self):
#        self._changeDirToBuild()
#        runShell('''\
# \cmake \
# -G"%s" \
# -DBUILD_SHARED_LIBS:BOOL=%s \
# -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
# ../%s''' % (DATA.mCMakeGenerator,
#            DATA.mBuildShared, 
#            DATA.mOSX_DEPLOYMENT_TARGET,
#            self.sourceFolder()))
#    def build(self):
#        CppComponent.build(self)
#        runShell('sudo make install')
#    def installPath(self):
#        #return '/usr/local/include/dcmtk'
#        return '' # ignore: use yum instead
#===============================================================================
# ---------------------------------------------------------

class ISB_DataStreaming(CppComponent):
    def name(self):
        self.mCurrentRevision = "361"
        return "ISB_DataStreaming"
    def help(self):
        return 'ISB GE Digital Interface stuff'
    def path(self):
        return DATA.mWorkingDir + "/ISB_DataStreaming"
    def _rawCheckout(self):
        self._changeDirToBase()
        runShell('svn co http://svn.isb.medisin.ntnu.no/DataStreaming/ -r%s %s %s' % (self.mCurrentRevision, self._svn_login_info(), self.sourceFolder()))
    def update(self):
        self._changeDirToSource()
#        runShell('svn up')
        runShell('svn up -r%s %s %s' % (self.mCurrentRevision, self._svn_login_info(), self.sourceFolder()))
    def configure(self):
        self._changeDirToBuild()
        runShell('''\
cmake \
-G"%s" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DVTK_DIR:PATH="%s" \
-DDATASTREAMING_USE_HDF:BOOL=OFF \
-DDATASTREAMING_USE_TRACKING:BOOL=OFF \
-DDATASTREAMING_USE_SC_DICOM_LOADERS:BOOL=OFF \
-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
../%s''' % (DATA.mCMakeGenerator,
            DATA.m32bitCompileCMakeOption, 
            DATA.mBuildType, 
            DATA.mBuildShared, 
            VTK().buildPath(), 
            DATA.mOSX_DEPLOYMENT_TARGET,
            self.sourceFolder()+"/vtkDataStreamClient/")
            )
        # add xcode project here if needed
    def _svn_login_info(self):
        '''
        return login info to be added as arguments to the svn co and up calls.
        '''
        if DATA.mISBpassword == "":
            return '--username sintef %s' % self.mCurrentRevision
        else:
            return '--non-interactive --username sintef --password %s' % DATA.mISBpassword
    # ---------------------------------------------------------

#===============================================================================
# class SSC(CppComponent):
#    def name(self):
#        return "SSC"
#    def help(self):
#        return 'SintefSonowandCooperation'
#    def path(self):
#        return DATA.mWorkingDir + "/SSC"
#    def _rawCheckout(self):
#        self._changeDirToBase()
# #        runShell('svn co https://wush.net/svn/ssc %s' % self.sourceFolder())
#        runShell('git clone git@github.com:SINTEFMedisinskTeknologi/SSC.git')
#    def update(self):
#        self._changeDirToSource()
#        runShell('git pull')
#        runShell('git checkout')   
# #        runShell('svn up')
#    def configure(self):
#        self._changeDirToBuild()
#        runShell('''\
# cmake \
# -G"%s" \
# %s \
# -DCMAKE_BUILD_TYPE:STRING=%s \
# -DBUILD_SHARED_LIBS:BOOL=%s \
# -DVTK_DIR:PATH="%s" \
# ../%s''' % (DATA.mCMakeGenerator, 
#            DATA.m32bitCompileCMakeOption, 
#            DATA.mBuildType, 
#            DATA.mBuildShared, 
#            VTK().buildPath(), 
#            self.sourceFolder())
#            )
#        # add xcode project here if needed
#===============================================================================
# ---------------------------------------------------------

class CustusX3(CppComponent):
    def name(self):
        return "CustusX3"
    def help(self):
        return 'CustusX from SINTEF'
    def path(self):
        return DATA.mWorkingDir + "/CustusX3"
    def _rawCheckout(self):
        self._changeDirToBase()
#        runShell('git clone ssh://%s@medtekserver.sintef.no/git/CustusX3.git CustusX3' % DATA.mServerUser)
        runShell('git clone git@github.com:SINTEFMedisinskTeknologi/CustusX3.git')
        #runShell('git clone https://%s:%s@github.com/SINTEFMedisinskTeknologi/CustusX3.git' % (DATA.mGitHubUser, DATA.mGitHubPassword)) # No need to setup ssh keys using this method
        #runShell('git clone ssh://%s@medtekserver.sintef.no/git/CustusX3.git CustusX3' % DATA.mServerUser)
        self._changeDirToSource()
        runShell('git submodule update --init --recursive externals/ssc')
        runShell('git submodule update --init --recursive data')
    def update(self):
        self._changeDirToSource()
        runShell('git checkout master')
        runShell('git pull')
        runShell('git submodule update --init --recursive')
    def configure(self):
        self._changeDirToBuild()
        runShell('''\
cmake \
-G"%s" \
%s \
-DCMAKE_CXX_FLAGS:STRING=-Wno-deprecated \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DBUILD_OPEN_IGTLINK_SERVER=true \
-DITK_DIR:PATH="%s" \
-DVTK_DIR:PATH="%s" \
-DIGSTK_DIR:PATH="%s" \
-DOpenIGTLink_DIR:PATH="%s" \
-DOpenCV_DIR:PATH="%s" \
-DULTERIUS_INCLUDE_DIR:PATH="%s" \
-DULTERIUS_LIBRARY:FILEPATH="%s" \
-DCX_USE_TSF:BOOL=OFF \
-DSSC_USE_DCMTK:BOOL=OFF \
-DTube-Segmentation-Framework_DIR:PATH="%s" \
-DSSC_BUILD_EXAMPLES="%s" \
-DBUILD_TESTING="%s" \
-DCOTIRE_ADD_UNITY_BUILDS="%s" \
-DCOTIRE_ENABLE_PRECOMPILED_HEADERS="%s" \
-DGEStreamer_DIR:PATH="%s" \
-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
-DSSC_USE_GCOV:BOOL=%s \
%s \
../%s''' % (DATA.mCMakeGenerator,
            DATA.m32bitCompileCMakeOption, 
            DATA.mBuildType, DATA.mBuildShared, 
            ITK().buildPath(), 
            VTK().buildPath(), 
            IGSTK().buildPath(),     
            OpenIGTLink().buildPath(), 
            OpenCV().buildPath(),
            UltrasonixSDK().includePath(),
            UltrasonixSDK().libFile(),
            TubeSegmentationFramework().buildPath(),
            DATA.mBuildSSCExamples,
            DATA.mBuildTesting,
            DATA.mUseCotire,
            DATA.mUseCotire,
            ISB_DataStreaming().buildPath(),
            DATA.mOSX_DEPLOYMENT_TARGET,
            DATA.mCoverage,
            DATA.mCMakeArgs,
            self.sourceFolder() )
            )
        #TODO add xcode project here if needed?
# ---------------------------------------------------------

class UltrasonixSDK(CppComponent):
    def name(self):
        return "UltrasonixSDK"
    def help(self):
        return 'UltrasonixSDK'
    def includePath(self):
        return self.path() + "/" + self.sourceFolder() + "/ulterius/inc"
    def libFile(self):
        return self.path() + "/" + self.sourceFolder() + "/ulterius/lib/ulterius.lib"
    def path(self):
        return DATA.mExternalDir + "/UltrasonixSDK"
    def _rawCheckout(self):
        changeDir(self.path())
        runShell('git clone ssh://%s@medtek.sintef.no/git/UltrasonixSDK.git' % DATA.mServerUser)
    def update(self):
        self._changeDirToSource()
        runShell('git checkout v5.7.4')
        runShell('git pull')
    def configure(self):
        pass
    def build(self):
        pass
# ---------------------------------------------------------

class TubeSegmentationFramework(CppComponent):
   def name(self):
       return "Tube-Segmentation-Framework"
   def help(self):
       return 'Tube-Segmentation-Framework'
   def path(self):
       return DATA.mWorkingDir + "/Tube-Segmentation-Framework"
   def _rawCheckout(self):
       changeDir(self.path())
       runShell('git clone git@github.com:SINTEFMedisinskTeknologi/Tube-Segmentation-Framework.git')
       self._changeDirToSource()
       runShell('git submodule update --init --recursive')
   def update(self):
       self._changeDirToSource()
       runShell('git checkout master')
       runShell('git pull')
   def configure(self):
       self._changeDirToBuild()
       runShell('''\
cmake \
-G"%s" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DUSE_C++11=false \
-DSIPL_USE_GTK=false \
../%s''' % (DATA.mCMakeGenerator, 
           DATA.m32bitCompileCMakeOption, 
           DATA.mBuildType, 
           DATA.mBuildShared,  
           self.sourceFolder())
           )
        
# ---------------------------------------------------------

# ===============================================================================
class CustusX3Data(Component):
    def name(self):
        return "CustusX3-Data"
    def help(self):
        return 'data files for CustusX'
    def path(self):
        custusx = CustusX3()
        return custusx.path() + "/" + custusx.sourceFolder()
        #return DATA.mWorkingDir + "/CustusX3"
    def sourceFolder(self):
        return 'data'
    def _rawCheckout(self):
        changeDir(self.path())
        runShell('git clone ssh://medtek.sintef.no//Volumes/medtek_HD/git/Data.git %s' % self.sourceFolder())
#runShell('svn co svn+ssh://%s@cxserver.sintef.no/svn/Repository/data' % DATA.mServerUser)
    def update(self):
        changeDir(self.path()+'/'+self.sourceFolder())
        runShell('git checkout master')
        runShell('git pull')
    def configure(self):
        pass
    def build(self):
        pass
# ---------------------------------------------------------


class Controller(object):
    '''
    A command line program that parses options and arguments,
    then performs the requested operations on the selected
    components.
    '''
    def __init__(self):
        '''
    Initialize and run the controller
    '''
        self.libraries = [
                     #CppUnit(),
                     ITK(),
                     VTK(),
                     OpenCV(),
                     OpenIGTLink(),
                     IGSTK(),
                     #DCMTK(),
                     #SSC(),
                     ISB_DataStreaming(),
                     UltrasonixSDK(),
                     TubeSegmentationFramework(),
                     CustusX3(),
                     CustusX3Data()
                     ]
        self.libnames = [lib.name() for lib in self.libraries]
        
        self.optionParser = self._createOptionParser();
        self._parseCommandLine()
    
    def _createOptionParser(self):
        description='''
Installer script for CustusX and its components.
All components given as arguments will be installed.
If no arguments are given, all components will be chosen.

Available components are:
%s.
''' % ', '.join(self.libnames)
        
        #Create instance of OptionParser Module, included in Standard Library
        p = optparse.OptionParser(description=description,
                                    version='%prog version 0.2',
                                    usage= '%prog [options] [components]')
        # p.add_option('--verbose', '-v',
        #             action = 'store_true',
        #             help='prints verbosely',
        #             default=False)
        # p.add_option('--ignore-existing', '-i',
        #             action='store_true',
        #             help='ignore components already created',
        #             default=False)
        p.add_option('--checkout', '--co',
                     action='store_true',
                     help='checkout all selected components',
                     default=False)
        p.add_option('--configure_clean', '--conf_c',
                     action='store_true',
                     help='delete build folder(s), configure all selected components',
                     default=False)
        p.add_option('--configure', '--conf',
                     action='store_true',
                     help='configure all selected components',
                     default=False)
        p.add_option('--build', '--make', '-b',
                     action='store_true',
                     help='build all selected components',
                     default=False)
        p.add_option('--all', '-a',
                     action='store_true',
                     help='select all components',
                     default=False)
        p.add_option('--full', '-f',
                     action='store_true',
                     help='checkout, configure, make',
                     default=False)
        p.add_option('--clean', '-c',
                     action='store_true',
                     help='make clean',
                     default=False)
        p.add_option('--threads',
                     '-j',
                     action='store',
                     type='int',
                     help='number of make threads',
                     dest='makethreads',
                     default=1)
        p.add_option('--static',
                     action='store_true',
                     help='build static libraries',
                     default=False)
        p.add_option('--b32',
                     action='store_true',
                     help='build 32 bit',
                     default=False)
        #TODO create option to select cmake 
        p.add_option('--xcode',
                     action='store_true',
                     help='generate xcode targets (Mac)',
                     default=False)
        p.add_option('--jom',
                     action='store_true',
                     help='generate jom targets (Windows)',
                     default=False)
        p.add_option('--user',
                     '-u',
                     action='store',
                     type='string',
                     help='user name for sintef login',
                     #dest='password',
                     default="")
        p.add_option('--password',
                     '-p',
                     action='store',
                     type='string',
                     help='password to send to scripts',
                     #dest='password',
                     default="")
        p.add_option('--dummy', '-d',
                     action='store_true',
                     help='execute script without calling any shell commands',
                     default=False)
        p.add_option('--build_type', '-t',
                     action='store',
                     type='string',
                     help='build type: (Debug,Release,RelWithDebInfo)',
                     default='Debug')
        p.add_option('--silent_mode', '-s',
                     action='store_true',
                     help='execute script without user interaction',
                     default=False)
        p.add_option('--isb_password',
                     action='store',
                     type='string',
                     help='password for svn sintef user at isb',
                     default="")
        p.add_option('--coverage',
                     '--cv',
                     action='store_true',
                     help='gcov code coverage',
                     default=False)
        p.add_option('--external_dir',
                     action='store',
                     type='string',
                     help='specify external folder, default=%s'%DATA.mExternalDir,
                     default=DATA.mExternalDir)
        p.add_option('--working_dir',
                     action='store',
                     type='string',
                     help='specify work folder, default=%s'%DATA.mWorkingDir,
                     default=DATA.mWorkingDir)
        p.add_option('--cmake_args',
                     action='store',
                     type='string',
                     help='additional arguments to ALL cmake calls',
                     default="")
        return p
    
    def _parseCommandLine(self):
        '''
        Parse the options and arguments from the command line
        '''
        options, arguments = self.optionParser.parse_args()
        DATA.options = options
        
        DATA.mBuildType = options.build_type
        # if options.basic:
        # downloadDiskImages()
        # return
        # if options.verbose:
        # shell.VERBOSE = True
        if options.dummy:
            print 'Running DUMMY mode: no shell executed.'
            shell.DUMMY = True
        if options.silent_mode:
            print 'Running silent mode: no user interaction needed.'
            DATA.mSilent_mode = True
        if options.password:
            print 'Set password (not working well): ', options.password
            shell.password = options.password
        if options.user:
            print 'Set server user: ', options.user
            DATA.mServerUser = options.user
        if options.static:
            DATA.mBuildShared = 'OFF'
            DATA.mBuildFolder = DATA.mBuildFolder + "_static"
            print 'Build shared: %s' % DATA.mBuildShared
        if options.b32:
            DATA.m32bitCompileCMakeOption = "-DCMAKE_OSX_ARCHITECTURES=i386"
            DATA.mBuildFolder = DATA.mBuildFolder + "32"
            print 'Build 32 bit'
        if options.xcode:
            DATA.mCMakeGenerator = "Xcode"
            DATA.mBuildFolder = DATA.mBuildFolder + "_xcode"
            print 'Generate xcode'
        if options.jom:
            DATA.mCMakeGenerator = 'NMake Makefiles JOM'
            DATA.mBuildFolder = DATA.mBuildFolder + "_jom"
            print 'Generate jom makefiles'
        if options.isb_password:
            DATA.mISBpassword = options.isb_password
        if options.coverage:
            DATA.mCoverage = 'ON'
        if options.external_dir:
            DATA.mExternalDir = options.external_dir
        if options.working_dir:
            DATA.mWorkingDir = options.working_dir
        if options.cmake_args:
            DATA.mCMakeArgs = options.cmake_args

        
        #TODO can be wrong for external libs as they use DATA.mBuildExternalsType!
        DATA.mBuildFolder = DATA.mBuildFolder + "_" + DATA.mBuildType 
        
        useLibNames = [val for val in self.libnames if val in arguments]
        
        if options.all:
            useLibNames = self.libnames
            
        useLibs = [lib for lib in self.libraries if lib.name() in useLibNames]
        
        #Windows do not allow linking between different build types
        if(DATA.PLATFORM == 'Windows'):
            DATA.mBuildExternalsType = DATA.mBuildType
                  
        # display help if no components selected
        if len(useLibs)==0:
            self.optionParser.print_help()
        else:
            self._performRequestedOperations(options, useLibs)

    def _performRequestedOperations(self, options, useLibs):
        '''
        checkout, configure, build
        '''
        # info
	self._printSettings()

        print 'Use the following components:', [val.name() for val in useLibs]
        if (not DATA.mSilent_mode):
            raw_input("\nPress enter to continue or ctrl-C to quit:")

        if options.full or options.checkout:
            for lib in useLibs:
                print '\n================== checkout ', lib.name(), '========================'
                lib.checkout()
        if options.full or options.checkout:
            for lib in useLibs:
                print '\n================== update ', lib.name(), '========================'
                lib.update()
        if options.configure_clean:
            for lib in useLibs:
                print '\n================== reset build folder ', lib.name(), '==================='
                lib.reset()
        if options.full or options.configure or options.configure_clean:
            for lib in useLibs:
                print '\n================== configure ', lib.name(), '========================'
                lib.configure()
        if options.clean:
            for lib in useLibs:
                print '\n================== make clean ', lib.name(), '========================'
                lib.makeClean()
        if options.full or options.build:
            for lib in useLibs:
                print '\n================== build ', lib.name(), '========================'
                lib.build()
    # ---------------------------------------------------------
    def _printSettings(self):
        print ''
        print 'Settings:'
        print '	User:', DATA.mUser
        print '	Server User:', DATA.mServerUser
        print '	PLATFORM:', DATA.PLATFORM
        print '	ISBpassword:', DATA.mISBpassword
        print '	RootDir:', DATA.mRootDir
        print '	ExternalDir:', DATA.mExternalDir
        print '	WorkingDir:', DATA.mWorkingDir
        print '	Silent_mode:', DATA.mSilent_mode
        print '	BuildShared:', DATA.mBuildShared
        print '	BuildType:', DATA.mBuildType
        print '	BuildExternalsType:', DATA.mBuildExternalsType
        print '	BuildTesting:', DATA.mBuildTesting
        print '	CMakeGenerator:', DATA.mCMakeGenerator
        print '	Coverage:', DATA.mCoverage
        print ''

def main():
    Controller()

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()


