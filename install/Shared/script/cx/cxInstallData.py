#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#
# Description:
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
        self.m32bit = False
        
        self._initPaths()
        self.mISBpassword = ""
        self.mBuildShared = True   # build as shared or static libraries
        self.setBuildType("Debug") 
        self.threads = 1
        
        self.mBuildSSCExamples = True
        self.mBuildTesting = True
        self.mUseCotire = False
        self.mOSX_DEPLOYMENT_TARGET = "10.6" # Deploy for OSX 10.6 Snow Leopard and later
        if (platform.system() == 'Windows'):
            self.mCMakeGenerator = 'Eclipse CDT4 - NMake Makefiles' # need to surround with ' ' instead of " " on windows for it to work
            self.mBuildSSCExamples = False
            self.mBuildTesting = False
            self.mUseCotire = True
        else:
            self.mCMakeGenerator = "Eclipse CDT4 - Unix Makefiles" # or "Xcode". Use -eclipse or -xcode from command line. Applies only to workspace projects.
        self.mBuildExAndTest = False
        self.mCoverage = False
        self.mDoxygen = False

    def printSettings(self):
        print ''
        print 'Settings:'
        print '    User:', getpass.getuser()
        print '    platform:', platform.system()
        print '    ISBpassword:', self.mISBpassword
        print '    RootDir:', self.mRootDir
        print '    ExternalDir:', self.getExternalPath()
        print '    WorkingDir:', self.getWorkingPath()
        print '    CMakeGenerator:', self.mCMakeGenerator
        print '    BuildShared:', self.mBuildShared
        print '    BuildType:', self.mBuildType
        print '    BuildExternalsType:', self.mBuildExternalsType
        print '    BuildTesting:', self.mBuildTesting
        print '    Coverage:', self.mCoverage
        print '    Threads:', self.threads
        print '    32 bit:', self.m32bit
        print ''

    def setCMakeGenerator(self, value):
        self.mCMakeGenerator = value    
    
    def setBuild32(self, value):
        self.m32bit = value        
    
    def setBuildType(self, value):
        allowedValues = ['Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel']
        if value not in allowedValues:
            raise Exception("Error: %s is not a valid build type. Chose one of %s" % (value, allowedValues))
        self.mBuildType = value
        self.mBuildExternalsType = "Release" # used for all non-cx libs, this because we want speed even in debug...
        if(platform.system() == 'Windows'): #Windows do not allow linking between different build types
            self.mBuildExternalsType = self.mBuildType            
    
    def getBuildType(self):
        return self.mBuildType
    
    def getBuildExternalsType(self):
        return self.mBuildExternalsType
    
    def getWorkingPath(self):
        return "%s/%s" % (self.mRootDir, self.mWorkingFolder)
    
    def getExternalPath(self):
        return "%s/%s" % (self.mRootDir, self.mExternalFolder)                        
    
    def setBuildShared(self, value):
        self.mBuildShared = value
    
    def getBuildFolder(self):
        retval = 'build'
        retval = retval + '_' + self.mBuildType
        if not self.mBuildShared:
            retval = retval + "_static"
        if self.m32bit == True:
            retval = retval + "32"
        if self.mCMakeGenerator == "Xcode":
            retval = retval + "_xcode"
        if self.mCMakeGenerator == 'NMake Makefiles JOM':
            retval = retval + "_jom"
        return retval
            
    def _initPaths(self):                
        if platform.system() == 'Windows':
            self.mRootDir = "C:/Dev"
        else:
            self.mRootDir = os.path.expanduser("~") + "/dev" #+ getpass.getuser() - use new default
        # external dir: Used as base dir for all externals, such as VTK, ITK, ...
        self.mExternalFolder = "external"
        # working dir: Used as base dir for Custus and other of our 'own' projects
        self.mWorkingFolder = "working"

    def _convertOnOffToBool(self, value):
        if value==True or value=='ON':
            return True
        else:
            return False

    def _convertBoolToOnOff(self, value):
        if value==True or value=='ON':
            return 'ON'
        else:
            return 'OFF'

# ---------------------------------------------------------

