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
import argparse
import re
import sys
import os.path
import urllib
import getpass
import platform

from cx.cxShell import *
import cx.cxInstallData
import cx.cxComponents
import cx.cxComponentAssembly

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
        self.assembly = cx.cxComponentAssembly.LibraryAssembly()
        self.optionParser = self._createOptionParser();
        self._parseCommandLine()
    
    def _createOptionParser(self):
        description='''
Installer script for CustusX and its components.
All components given as arguments will be installed.
If no arguments are given, all components will be chosen.

Available components are:
   %s.
''' % '\n   '.join(self.assembly.getLibnames())
        
        p = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=description)
        p.add_argument('--checkout', '--co', action='store_true', default=False, help='checkout all selected components')
        p.add_argument('--configure_clean', action='store_true', default=False, help='delete build folder(s), configure all selected components')
        p.add_argument('--configure', '--conf', action='store_true', default=False, help='configure all selected components')
        p.add_argument('-b', '--build', '--make', action='store_true', default=False, help='build all selected components')
        p.add_argument('-a', '--all', action='store_true', default=False, help='select all components')
        p.add_argument('-f', '--full', action='store_true', default=False, help='checkout, configure, make')
        p.add_argument('-c', '--clean', action='store_true', default=False, help='make clean')
        p.add_argument('-j', '--threads', type=int, default=1, help='number of make threads')
        p.add_argument('--static', action='store_true', default=False, help='build static libraries')
        p.add_argument('--b32', action='store_true', default=False, help='build 32 bit')
        p.add_argument('--xcode', action='store_true', default=False, help='generate xcode targets (Mac)')
        p.add_argument('--jom', action='store_true', default=False, help='generate jom targets (Windows)')
#        p.add_argument('--password', '-p', default="", help='password to send to scripts')
        p.add_argument('-d', '--dummy', action='store_true', default=False, help='execute script without calling any shell commands')
        p.add_argument('-t', '--build_type', choices=['Debug','Release','RelWithDebInfo'], help='build type', default='Debug')
        p.add_argument('-s', '--silent_mode', action='store_true', default=False, help='execute script without user interaction')
        p.add_argument('--isb_password', default="", help='password for svn sintef user at isb')
        p.add_argument('--coverage', action='store_true', default=False, help='gcov code coverage')
        p.add_argument('--doxygen', action='store_true', default=False, help='build doxygen documentation')
#        p.add_argument('--external_dir', default=self.controlData.mExternalDir, help='specify external folder, default=%s'%self.controlData.mExternalDir)
#        p.add_argument('--working_dir', default=self.controlData.mWorkingDir, help='specify work folder, default=%s'%self.controlData.mWorkingDir)
        rootdir = self.assembly.controlData.mRootDir
        p.add_argument('--root_dir', default=rootdir, help='specify root folder, default=%s'%rootdir)
#        p.add_argument('--cmake_args', default="", help='additional arguments to ALL cmake calls')
        p.add_argument('components', nargs='*', help='list of all components to process')
        return p
    
    def _parseCommandLine(self):
        options = self.optionParser.parse_args()
        self._fillControlDataFromOptions(options)
        shell.setDummyMode(options.dummy)
        
        if options.all:
            self.assembly.selectAllLibraries()
        else:
            self.assembly.selectLibraries(options.components)

        # display help if no components selected
        if len(self.assembly.getSelectedLibraries())==0:
            self.optionParser.print_help()
            return

        # info + verification
        self.assembly.controlData.printSettings()
        print 'Use the following components:\n ', '\n  '.join(self.assembly.getSelectedLibraries())
        self._promptToContinue(options.silent_mode)
        
#        self.assembly.selectLibrariesFromNames(selectedLibnames)
        self.assembly.process(checkout = options.full or options.checkout, 
                             configure_clean = options.configure_clean, 
                             configure = options.full or options.configure, 
                             clean = options.clean, 
                             build = options.full or options.build)
            
    def _fillControlDataFromOptions(self, options):
        data = self.assembly.controlData
        print "*"*100
        print options
        print "*"*100
        
        data.mRootDir = options.root_dir 
        data.setBuildType(options.build_type)

        data.setBuildShared(not options.static)
        data.setBuild32(options.b32)

        if options.xcode:
            data.setCMakeGenerator("Xcode")
        if options.jom:
            data.setCMakeGenerator("NMake Makefiles JOM")
        
        data.mISBpassword = options.isb_password
        data.mCoverage = options.coverage
        data.mDoxygen = options.doxygen
        data.threads = options.threads
                
    def _promptToContinue(self, silent_mode):
        if not silent_mode:
            raw_input("\nPress enter to continue or ctrl-C to quit:")

def main():
    Controller()

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()


