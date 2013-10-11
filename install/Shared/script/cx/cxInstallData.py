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
# default folder setup
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
#    |-----working
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
import argparse
import pprint
import cxArgParse
    
class Common(object):
    '''
    Container for common data.
    Modify these to change behaviour
    '''
    def __init__(self):
        self.root_dir = None
        self.m32bit = False
        self._initPaths()
        self.isb_password = ""
        self.static = False # build as shared or static libraries
        self.jom = False
        if platform.system() == 'Windows':
            self.static = True
            self.jom = True
        self.setBuildType("Debug") 
        self.threads = 1
        self.xcode = False
        self.mBuildSSCExamples = True
        self.mBuildTesting = True
        self.mUseCotire = False
        self.mUseOpenCL = True
        self.mOSX_DEPLOYMENT_TARGET = "10.6" # Deploy for OSX 10.6 Snow Leopard and later
        if (platform.system() == 'Windows'):
            self.mBuildSSCExamples = False
            self.mBuildTesting = True
            self.mUseCotire = False
        if (platform.system() == "Darwin"):
            self.mUseOpenCL = False # Turn off OpenCL for Mac as Jenkins tests are run on olevs mac, and OpenCL code don't work there yet
        self.mBuildExAndTest = False
        self.mCoverage = False
        self.mDoxygen = False
        self.git_tag = None # if none, use branch master

    def printSettings(self):
        print ''
        print 'Settings:'
        print '    User:', getpass.getuser()
        print '    platform:', platform.system()
        print '    ISBpassword:', self.isb_password
        print '    RootDir:', self.getRootDir()
        print '    ExternalDir:', self.getExternalPath()
        print '    WorkingDir:', self.getWorkingPath()
        print '    CMakeGenerator:', self.getCMakeGenerator()
        print '    BuildShared:', self.getBuildShared()
        print '    BuildType:', self.getBuildType()
        print '    BuildExternalsType:', self.getBuildExternalsType()
        print '    BuildTesting:', self.mBuildTesting
        print '    Coverage:', self.mCoverage
        print '    Threads:', self.threads
        print '    32 bit:', self.m32bit
        print '    git tag:', self.git_tag
        print '    OpenCL:', self.mUseOpenCL
        print ''

    def getArgParser_root_dir(self):
        p = cxArgParse.ArgumentParser(add_help=False)
        p.add_argument('--root_dir', default=self.root_dir, help='specify root folder, default=%s' % self.root_dir)
        p.add_argument('--print_control_data', action='store_true', default=False, help='Print all control data at startup')
        return p

    def getArgParser_core_build(self):
        p = cxArgParse.ArgumentParser(add_help=False)
        p.add_argument('-i', '--isb_password', default="not set", metavar='PASSWORD', dest='isb_password', help='password for ISB GE Connection module')
        p.add_argument('-j', '--threads', type=int, default=1, dest='threads', help='number of make threads')
        p.add_argument('-g', '--git_tag', default=None, metavar='TAG', dest='git_tag', help='git tag to use when checking out CustusX. None means checkout master branch.')
        p.add_argument('-t', '--build_type', default=self.build_type, dest='build_type', choices=self._getAllowedBuildTypes(), help='build type')
        p.add_boolean_inverter('--b32', default=self.m32bit, dest='m32bit', help='Build 32 bit.')
        p.add_boolean_inverter('--static', default=self.static, dest='static', help='Link statically.')        
        if platform.system() == 'Windows':
            p.add_boolean_inverter('--jom', default=self.jom, dest='jom', help='Use jom to build.')
        if platform.system() == 'Darwin':
            p.add_boolean_inverter('--xcode', default=self.xcode, dest='xcode', help='generate xcode targets')
        return p

    def getArgParser_extended_build(self):
        p = cxArgParse.ArgumentParser(add_help=False)
        p.add_boolean_inverter('--coverage', default=self.mCoverage, dest='mCoverage', help='gcov code coverage')
        p.add_boolean_inverter('--doxygen', default=self.mDoxygen, dest='mDoxygen', help='build doxygen documentation')
        return p

    def applyCommandLine(self, arguments):
        arguments = self.getArgParser_root_dir().parse_known_args(args=arguments, namespace=self)[1]
        arguments = self.getArgParser_core_build().parse_known_args(args=arguments, namespace=self)[1]
        arguments = self.getArgParser_extended_build().parse_known_args(args=arguments, namespace=self)[1]
        if self.print_control_data:
            #self.printSettings()
            pprint.pprint(vars(self))
        return arguments

    def getCMakeGenerator(self):
        if self.xcode:
            return "Xcode"
        if self.jom:
            return "NMake Makefiles JOM"
        if platform.system() == 'Windows':
            return 'Eclipse CDT4 - NMake Makefiles' # need to surround with ' ' instead of " " on windows for it to work
        return "Eclipse CDT4 - Unix Makefiles" 
    
    def setBuild32(self, value):
        self.m32bit = value        
    
    def _getAllowedBuildTypes(self):
        return ['Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel']

    def setBuildType(self, value):
        allowedValues = self._getAllowedBuildTypes()
        if value not in allowedValues:
            raise Exception("Error: %s is not a valid build type. Chose one of %s" % (value, allowedValues))
        self.build_type = value
    
    def getBuildShared(self):
        return not self.static # self._buildShared
    def getBuildType(self):
        return self.build_type
    
    def getBuildExternalsType(self):
        if(platform.system() == 'Windows'): # Windows do not allow linking between different build types
            return self.build_type            
        return "Release" # used for all non-cx libs, this because we want speed even in debug...
    
    def setRootDir(self, root_dir):
        if root_dir:
            self.root_dir = root_dir
            
    def getRootDir(self):
        return self.root_dir
    
    def getWorkingPath(self):
        return "%s/%s" % (self.root_dir, self._workingFolder)
    
    def getExternalPath(self):
        return "%s/%s" % (self.root_dir, self._externalFolder)                        
    
    def setBuildShared(self, value):
        self.static = not value
        #self._buildShared = value
    
    def getBuildFolder(self):
        retval = 'build'
        retval = retval + '_' + self.build_type
        #if not self._buildShared:
        if self.static == True:
            retval = retval + "_static"
        if self.m32bit == True:
            retval = retval + "32"
        if self.xcode == True:
            retval = retval + "_xcode"
        if self.jom == True:
            retval = retval + "_jom"
        return retval
            
    def _initPaths(self):                
        if platform.system() == 'Windows':
            self.root_dir = "C:/Dev"
        else:
            self.root_dir = os.path.expanduser("~") + "/dev" #+ getpass.getuser() - use new default
        # external dir: Used as base dir for all externals, such as VTK, ITK, ...
        self._externalFolder = "external"
        # working dir: Used as base dir for Custus and other of our 'own' projects
        self._workingFolder = "working"
        
    def getGitTag(self):
        if self.git_tag == "":
            return None
        return self.git_tag

# ---------------------------------------------------------

