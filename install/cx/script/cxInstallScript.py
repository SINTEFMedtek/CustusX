#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#
# Run cxInstaller --help to get usage info.
#             
#################################################             

from __future__ import print_function
from __future__ import absolute_import
import subprocess
import argparse
import re
import sys
import os.path
import urllib.request, urllib.parse, urllib.error
import getpass
import platform

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter

# Only used on Windows
from ..windows_env import apply_msvc_env, verify_msvc_tools
from ..windows_env import apply_custusx_extra_env


import cx.build.cxInstallData
import cx.build.cxComponents
import cx.build.cxComponentAssembly

from . import cxBuildScript

class Controller(cxBuildScript.BuildScript):
    '''
    '''

    def getDescription(self):                  
        return '''
Installer script for CustusX and its components.

Root folder is cxrepo/../.. = %s

Available components are:
   %s.
''' % (self.controlData().getRootDir(),
       '\n   '.join(self.assembly.getLibnames()))
       
    def setDefaults(self):                
        super(Controller, self).setDefaults()
        self.controlData().setBuildType("Debug")
        shell.setRedirectOutput(False)

    def addArgParsers(self):        
        super(Controller, self).addArgParsers()
        self.additionalParsers.append(self.controlData().getArgParser_core_build())
        self.additionalParsers.append(self.controlData().getArgParser_extended_build())
        self.additionalParsers.append(self.getArgParser())
       
    def applyArgumentParsers(self, arguments):
        arguments = super(Controller, self).applyArgumentParsers(arguments)
        (self.options, arguments) = self.getArgParser().parse_known_args(arguments)
        print('Options: ', self.options)
        return arguments

    def getArgParser(self):
        p = argparse.ArgumentParser(add_help=False)
        p.add_argument('--checkout', action='store_true', dest='checkout', help='checkout all selected components')
        p.add_argument('--configure_clean', action='store_true', help='delete build folder(s), configure all selected components')
        p.add_argument('--configure', '--conf', action='store_true', help='configure all selected components')
        p.add_argument('-m', '-b', '--make', action='store_true', help='build all selected components')
        p.add_argument('-a', '--all', action='store_true', help='select all components')
        p.add_argument('-f', '--full', action='store_true', help='checkout, configure, make')
        p.add_argument('-c', '--clean', action='store_true', help='make clean')
        p.add_argument('-s', '--silent_mode', action='store_true', help='execute script without user interaction')
        p.add_argument('components', nargs='*', help='list of all components to process')

        # Windows/MSVC environment options
        if platform.system() == 'Windows':
            p.add_argument('--msvc-env', choices=['auto', 'off'], default='auto',
                           help='On Windows, auto-initialize Visual Studio build environment (default: auto).')
            p.add_argument('--msvc-batch', default=os.environ.get('CUSTUSX_VS_ENV', None),
                           help='Explicit path to vcvars64.bat or VsDevCmd.bat (overrides detection).')
            p.add_argument('--prefer-env', choices=['vcvars', 'vsdev'], default='vcvars', help='Prefer vcvars64 or VsDevCmd (default: vcvars).')
            
            # Qt detection / configuration (Windows)
            p.add_argument('--qt-path', default=os.environ.get('CUSTUSX_QT_PATH', ''),
                           help='Override the Qt root (e.g., C:\\Qt\\5.15.2\\msvc2019_64).')
            p.add_argument('--qt-version', default=os.environ.get('CUSTUSX_QT_VERSION', '5.15.2'),
                           help='Qt version to auto-detect (default: 5.15.2).')
            p.add_argument('--qt-module', default=os.environ.get('CUSTUSX_QT_MODULE', 'msvc2019_64'),
                           help='Qt compiler module to auto-detect (default: msvc2019_64).')
            p.add_argument('--no-ninja', action='store_true', help='Do not force Ninja generator.')
            p.add_argument('--no-cxx17', action='store_true', help='Do not force C++17.')
            p.add_argument('--no-docs-off', action='store_true', help='Do not disable ITK/VTK documentation.')
            p.add_argument('--keep-sh', action='store_true', help='Do not set CMAKE_SH=CMAKE_SH-NOTFOUND.')
        
        return p

    def _promptToContinue(self, silent_mode):
        if not silent_mode:
            input("\nPress enter to continue or ctrl-C to quit:")

    def run(self):
        options = self.options
        assembly = self.assembly
                
        if options.all:
            assembly.selectAllLibraries()
        else:
            assembly.selectLibraries(options.components)

        # display help if no components selected
        if len(assembly.getSelectedLibraries())==0:
            PrintFormatter.printInfo("No libraries selected, exiting...")
            self.argumentParser.print_help()
            return

        # info + verification
        assembly.controlData.printSettings()
        libs = [lib for lib in assembly.libraries if lib.name() in assembly.selectedLibraryNames]
        text = ['%45s     %s' % (lib.name(), lib.repository()) for lib in libs]
        print('Use the following components:\n ', '\n  '.join(text))
        
        print('')
        print('*********************************************************************')
        print('The superbuild will check out and build in [repo/../..], expanded to:')
        print('')
        print('    %s' % assembly.controlData.getRootDir())
        print('')
        print('*********************************************************************')
        print('')
        
        self._promptToContinue(options.silent_mode)
        
        # --- Windows: Initialize MSVC environment (prefer vcvars64.bat) ---
        if platform.system() == 'Windows' and options.msvc_env == 'auto':
            try:
                used_batch = apply_msvc_env(prefer=options.prefer_env, explicit_batch=options.msvc_batch)
                PrintFormatter.printInfo(f"MSVC environment initialized via: {used_batch}")
                verify_msvc_tools()  # fail-fast if cl/cmake/ninja missing
                
                # Replicate Setup-CustusX.ps1 environment hints (Qt, Ninja, C++17, docs OFF)
                apply_custusx_extra_env(
                    qt_path=options.qt_path,
                    qt_version=options.qt_version,
                    qt_module=options.qt_module,
                    force_ninja=(not options.no_ninja),
                    set_cxx17=(not options.no_cxx17),
                    disable_docs=(not options.no_docs_off),
                    disable_git_sh=(not options.keep_sh),
                )

            except Exception as e:
                PrintFormatter.printWarning(
                    f"WARNING: Could not initialize Visual Studio environment automatically: {e}")
        
        assembly.process(checkout = options.full or options.checkout, 
                         configure_clean = options.configure_clean, 
                         configure = options.full or options.configure, 
                         clean = options.clean, 
                         build = options.full or options.make)        
        
        #self.cxBuilder.finish()
        PrintFormatter.finish()


