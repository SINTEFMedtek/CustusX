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

class Common:
    '''
Container for common data.
Modify these to change behaviour
'''
    def __init__(self):
        # user: Used to create root dir.
        self.mUser = bash.evaluate('whoami')
        # root dir: default base for external and working dir
        self.mRootDir = bash.evaluate('cd;pwd')+"/"+self.mUser
        # external dir: Used as base dir for all externals, such as VTK, ITK, ...
        self.mExternalDir = self.mRootDir + "/external_code"
        # working dir: Used as base dir for Custus and other of our 'own' projects
        self.mWorkingDir = self.mRootDir + "/workspace"
        # server user: Used for login to cx server etc.
        self.mServerUser = self.mUser
        # build as shared or static libraries
        self.mBuildShared = "ON" # Change to ON or OFF
        self.mBuildType = "Debug" # Debug, Release, RelWithDebInfo, MinSizeRel
        self.mBuildExternalsType = "Release" # used for all non-cx libs, this because we want speed even in debug...
        self.mBuildFolder = "build" # default build folder. This is auto-changed when using xcode or 32 bit.
        self.m32bitCompileCMakeOption = "" # use "-DCMAKE_OSX_ARCHITECTURES=i386" for 32 bit. Done automatically by settings --b32 from command line.
        self.mCMakeGenerator = "Eclipse CDT4 - Unix Makefiles" # or "Xcode". Use -eclipse or -xcode from command line. Applies only to workspace projects.
        self.mBuildExAndTest = "OFF"
    # ---------------------------------------------------------


class Bash:
    '''
wrapper for methods that call the shell.
Also keeps track of the current directory through changeDir()
'''
    def __init__(self):
        self.DUMMY = False
        self.VERBOSE = False
        self.CWD = '/' # remember directory
        self.password = ""

    def _runReal(self, cmd):
        'This function runs bash, no return, insert password'
        print '*** run:', cmd
        #p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=CWD)
        p = subprocess.Popen(cmd, shell=True, cwd=self.CWD)
# print "entering password ", self.password, " for ", cmd
        #p.communicate(self.password+"\n")
        p.communicate("") # seems to be needed in order to wait for password prompting...?

        #out = p.stdout.read().strip()
        #if out!="":
        # print out
             #return out #This is the stdout from the shell command
    
    def _runDummy(self, cmd):
        'dummy version of bash'
        if cmd=="whoami":
            return self.runReal(cmd)
        print "*** dummy run:", cmd
        return ""
    
    def run(self, cmd):
        'run a bash script'
        if self.DUMMY is True:
            return self._runDummy(cmd)
        else:
            return self._runReal(cmd)
           
    def evaluate(self, cmd):
     'This function takes bash commands and returns them'
    # print '*** eval run:', cmd
     p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=self.CWD)
     out = p.stdout.read().strip()
    # if out!="":
    # print out
     return out #This is the stdout from the shell command
    
    def changeDir(self, path):
     'mkdir + cd bash operations'
     runBash('mkdir -p '+path)
     self.CWD = path
        #runBash('cd '+path) - did not work, cd only operate on the local context
    # --------------------------------------------------------

# ---------------------------------------------------------
# global for bash interaction
bash = Bash()
# shortcuts
runBash = bash.run
changeDir = bash.changeDir
# global variable for data
DATA = Common()
# ---------------------------------------------------------

class Component:
    '''
Data for one Component
Superclass for all specific components,
which must implement this interface.
'''
    def name(self):
        'return name of component'
        #return self.mName
        raise "Not Implemented"
    def help(self):
        'description of item'
        return ''
    def path(self):
        'return path where component will be installed'
        raise "Not Implemented"
        #return self.mPath
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
        runBash('rm -R -f %s/%s' % (self.path(), self.buildFolder()))
    def build(self):
        self._changeDirToBuild()
        # the export DYLD... line is a hack to get shared linking to work on MacOS with vtk5.6
        # - http://www.mail-archive.com/paraview@paraview.org/msg07520.html
        # (add it to all project because it does no harm if not needed)
        runBash('''\
export DYLD_LIBRARY_PATH=`pwd`/bin; \
make -j%s
''' % str(DATA.options.makethreads))
    def makeClean(self):
        self._changeDirToBuild()
        runBash('make clean')
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
        runBash('git clone git://itk.org/ITK.git')
        self.update()
    def update(self):
        self._changeDirToSource()
        runBash('git checkout master')
        runBash('git pull')
        runBash('git checkout v3.20.0') # version working ok with IGSTK 4.2
        #runBash('git checkout v4.0rc03') # needed for gcc 4.6  
        #runBash('git checkout v4.1.0') # needed for gcc 4.6, but not ok with igstk.
    
    def configure(self):
        self._changeDirToBuild()
        runBash('''\
cmake \
-G"Eclipse CDT4 - Unix Makefiles" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DBUILD_TESTING=%s \
-DBUILD_EXAMPLES=%s \
../%s''' % (DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildShared, 
            DATA.mBuildExAndTest, 
            DATA.mBuildExAndTest, 
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
        runBash('git clone http://vtk.org/VTK.git')
        self.update()
    def update(self):
        self._changeDirToSource()
        runBash('git checkout master')
        runBash('git pull')
        runBash('git checkout v5.6.1')   # working with IGSTK 4.2
        #runBash('git checkout v5.8.0')   # needed for gcc 4.6, not goot on non-linux

    def configure(self):
        '''
Note: DVTK_REQUIRED_OBJCXX_FLAGS is required on v5.6 in order to avoid garbage-collection (!)
'''
        self._changeDirToBuild()
        runBash('''\
\cmake \
-G"Eclipse CDT4 - Unix Makefiles" \
%s \
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
../%s''' % (DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildExAndTest,
            DATA.mBuildExAndTest,
            DATA.mBuildShared, 
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
#    	runBash('svn co https://code.ros.org/svn/opencv/trunk/opencv OpenCV')
#        runBash('svn co https://code.ros.org/svn/opencv/branches/2.3/opencv OpenCV') #old location
        runBash('svn co http://code.opencv.org/svn/opencv/branches/2.3/opencv OpenCV')

    def update(self):
        pass
    def configure(self):
        self._changeDirToBuild()
        runBash('''\
cmake \
-G"Eclipse CDT4 - Unix Makefiles" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_EXAMPLES:BOOL=%s \
-DBUILD_TESTS:BOOL=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DWITH_CUDA:BOOL=OFF \
../%s''' % (DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildExAndTest,
            DATA.mBuildExAndTest,
            DATA.mBuildShared, 
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
        runBash('svn co http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink OpenIGTLink')
    def update(self):
        pass
    def configure(self):
        self._changeDirToBuild()
        runBash('''\
cmake \
-G"Eclipse CDT4 - Unix Makefiles" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DIGSTK_BUILD_EXAMPLES:BOOL=%s \
-DBUILD_TESTING:BOOL=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
../%s''' % (DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildExAndTest,
            DATA.mBuildExAndTest,
            DATA.mBuildShared, 
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
        runBash('''\
cvs -d :pserver:anonymous:igstk@public.kitware.com:/cvsroot/IGSTK login
cvs -d :pserver:anonymous@public.kitware.com:/cvsroot/IGSTK checkout -r "IGSTK-4-4" IGSTK
cvs -d :pserver:anonymous@public.kitware.com:/cvsroot/IGSTK logout
''')
	self._changeDirToSource()
	# this substitution makes IGSTK 4.4 work with ITK 4.0
	runBash('''\
sed -i "" s/'ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject'/'${ITK_LIBRARIES}'/g Source/CMakeLists.txt
''')
	# this substitution removes compilation of the dysfuct lib that we don't use.
	runBash('''\
sed -i "" s/'SUBDIRS( SceneGraphVisualization )'/'#SUBDIRS( SceneGraphVisualization )'/g Utilities/CMakeLists.txt
''')
    def update(self):
        pass
    def configure(self):
        self._changeDirToBuild()
        runBash('''\
cmake \
-G"Eclipse CDT4 - Unix Makefiles" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DIGSTK_USE_SceneGraphVisualization:BOOL=OFF \
-DIGSTK_DEVELOPMENT_VERSION:BOOL=ON \
-DIGSTK_BUILD_EXAMPLES:BOOL=OFF \
-DBUILD_TESTING:BOOL=OFF \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DIGSTK_BUILD_TESTING:BOOL=OFF \
-DITK_DIR:PATH="%s" \
-DVTK_DIR:PATH="%s" \
-DIGSTK_SERIAL_PORT_0="/Library/CustusX/igstk.links/cu.CustusX.dev0" \
-DIGSTK_SERIAL_PORT_1="/Library/CustusX/igstk.links/cu.CustusX.dev1" \
-DIGSTK_SERIAL_PORT_2="/Library/CustusX/igstk.links/cu.CustusX.dev2" \
-DIGSTK_SERIAL_PORT_3="/Library/CustusX/igstk.links/cu.CustusX.dev3" \
-DIGSTK_SERIAL_PORT_4="/Library/CustusX/igstk.links/cu.CustusX.dev4" \
-DIGSTK_SERIAL_PORT_5="/Library/CustusX/igstk.links/cu.CustusX.dev5" \
-DIGSTK_SERIAL_PORT_6="/Library/CustusX/igstk.links/cu.CustusX.dev6" \
-DIGSTK_SERIAL_PORT_7="/Library/CustusX/igstk.links/cu.CustusX.dev7" \
../%s''' % (DATA.m32bitCompileCMakeOption, 
            DATA.mBuildExternalsType, 
            DATA.mBuildShared, 
            ITK().buildPath(), 
            VTK().buildPath(), 
            self.sourceFolder())
            )
# ---------------------------------------------------------

class DCMTK(CppComponent):
    '''
    The only way to get 3.6.1 to work is to install it,
    thats the reason for the special treatment in this class.
    
    This might change in the future.
    '''
    def name(self):
        return "DCMTK"
    #def buildFolder(self):
    #    'in-source build because this is necessary for including an uninstalled DCMTK'
    #    return self.sourceFolder()    
    def help(self):
        return 'dcmtk.org'
    def path(self):
        return DATA.mExternalDir + "/DCMTK"

    def _rawCheckout(self):
        self._changeDirToBase()
#        runBash('git clone git://git.dcmtk.org/dcmtk DCMTK')
        # the commontk version of DCMTK compiles without problems on Mac.
        runBash('git clone git://github.com/commontk/DCMTK.git DCMTK')
        
        self.update()
    def update(self):
        self._changeDirToSource()
        runBash('git pull')
#        runBash('git checkout master')   
#        runBash('git checkout PUBLIC_360')  # 3.6.0 seems to have some issues on fedora 16.  

    def configure(self):
        self._changeDirToBuild()
        runBash('''\
\cmake \
-G"Eclipse CDT4 - Unix Makefiles" \
-DBUILD_SHARED_LIBS:BOOL=%s \
../%s''' % (DATA.mBuildShared, 
            self.sourceFolder()))
    def build(self):
        CppComponent.build(self)
        runBash('sudo make install')
    def installPath(self):
        return '/usr/local/include/dcmtk'
    # ---------------------------------------------------------

class SSC(CppComponent):
    def name(self):
        return "SSC"
    def help(self):
        return 'SintefSonowandCooperation'
    def path(self):
        return DATA.mWorkingDir + "/SSC"
    def _rawCheckout(self):
        self._changeDirToBase()
#        runBash('svn co https://wush.net/svn/ssc %s' % self.sourceFolder())
        runBash('git clone git@github.com:SINTEFMedisinskTeknologi/SSC.git')
    def update(self):
        self._changeDirToSource()
        runBash('git pull')
        runBash('git checkout')   
#        runBash('svn up')
    def configure(self):
        self._changeDirToBuild()
        runBash('''\
cmake \
-G"%s" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DVTK_DIR:PATH="%s" \
../%s''' % (DATA.mCMakeGenerator, DATA.m32bitCompileCMakeOption, DATA.mBuildType, DATA.mBuildShared, VTK().buildPath(), self.sourceFolder())
                    )
        # add xcode project here if needed
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
#        runBash('git clone ssh://%s@medtekserver.sintef.no/git/CustusX3.git CustusX3' % DATA.mServerUser)
        runBash('git clone git@github.com:SINTEFMedisinskTeknologi/CustusX3.git')
        #runBash('git clone ssh://%s@medtekserver.sintef.no/git/CustusX3.git CustusX3' % DATA.mServerUser)
        self._changeDirToSource()
#        runBash('git submodule init')
#        runBash('git submodule update')
        runBash('git submodule update --init --recursive externals/ssc')
        runBash('git submodule update --init --recursive data')
        #runBash('svn co svn+ssh://%s@cxserver.sintef.no/svn/Repository/CustusX3' % DATA.mServerUser)
    def update(self):
        self._changeDirToSource()
        runBash('git checkout master')
        runBash('git pull')
        runBash('git submodule update')
    def configure(self):
        self._changeDirToBuild()
        runBash('''\
cmake \
-G"%s" \
%s \
-DCMAKE_BUILD_TYPE:STRING=%s \
-DBUILD_SHARED_LIBS:BOOL=%s \
-DBUILD_OPEN_IGTLINK_SERVER=true \
-DITK_DIR:PATH="%s" \
-DVTK_DIR:PATH="%s" \
-DIGSTK_DIR:PATH="%s" \
-DOpenIGTLink_DIR:PATH="%s" \
-DOpenCV_DIR:PATH="%s" \
-DDCMTK_DIR:PATH="%s" \
../%s''' % (DATA.mCMakeGenerator, DATA.m32bitCompileCMakeOption, DATA.mBuildType, DATA.mBuildShared, ITK().buildPath(), VTK().buildPath(), IGSTK().buildPath(), OpenIGTLink().buildPath(), OpenCV().buildPath(), DCMTK().installPath(), self.sourceFolder() )
                    )
        # add xcode project here if needed
# ---------------------------------------------------------

class CustusX3Data(Component):
    def name(self):
        return "CustusX3-Data"
    def help(self):
        return 'data files for CustusX'
    def path(self):
        return DATA.mWorkingDir + "/CustusX3"
    def sourceFolder(self):
        return 'data'
    def _rawCheckout(self):
        changeDir(self.path())
        runBash('svn co svn+ssh://%s@cxserver.sintef.no/svn/Repository/data' % DATA.mServerUser)
    def update(self):
        changeDir(self.path()+'/'+self.sourceFolder())
        runBash('svn up')
    def configure(self):
        pass
    def build(self):
        pass
# ---------------------------------------------------------


class Controller():
    '''
A command line program that parses options and arguments,
then performs the requested operations on the selected
components.
'''
    def __init__(self):
        '''
initialize and run the controller
'''
        self.libraries = [
                     ITK(),
                     VTK(),
					 OpenCV(),
                     OpenIGTLink(),
                     IGSTK(),
                     #DCMTK(),
                     #SSC(),
                     CustusX3()
                     #CustusX3Data()
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
# action = 'store_true',
# help='prints verbosely',
# default=False)
# p.add_option('--ignore-existing', '-i',
# action='store_true',
# help='ignore components already created',
# default=False)
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
        p.add_option('--xcode',
                     action='store_true',
                     help='generate xcode targets',
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
#        p.add_option('--release', '-r',
#                     action='store_true',
#                     help='build release mode',
#                     default=False)
        p.add_option('--build_type', '-t',
                     action='store',
                     type='string',
                     help='build type: (Debug,Release,RelWithDebInfo)',
                     default='Debug')
        return p
    
    def _parseCommandLine(self):
        '''
parse the options and arguments from the command line
'''
        options, arguments = self.optionParser.parse_args()
        DATA.options = options
        
# if options.basic:
# downloadDiskImages()
# return
# if options.verbose:
# bash.VERBOSE = True
        if options.dummy:
            print 'Running DUMMY mode: no shell executed.'
            bash.DUMMY = True
        if options.password:
            print 'Set password (not working well): ', options.password
            bash.password = options.password
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
#        if options.release:
#            DATA.mBuildType = "Release"
#            print 'Build Release'
            
        DATA.mBuildType = options.build_type
        DATA.mBuildFolder = DATA.mBuildFolder + "_" + DATA.mBuildType
        
        useLibNames = [val for val in self.libnames if val in arguments]
        
        if options.all:
            useLibNames = self.libnames
            
        useLibs = [lib for lib in self.libraries if lib.name() in useLibNames]
                  
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
        print 'User:', DATA.mUser
        print 'Server User:', DATA.mServerUser
        print 'Root dir:', DATA.mRootDir
        print 'Use the following components:', [val.name() for val in useLibs]
        raw_input("\nPress enter to continue or ctrl-C to quit:")

        if options.full or options.checkout:
            for lib in useLibs:
                print '\n================== checkout ', lib.name(), '========================'
                lib.checkout()
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


def main():
    Controller()

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()

