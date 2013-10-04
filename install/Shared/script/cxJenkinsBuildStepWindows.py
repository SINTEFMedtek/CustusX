#!/usr/bin/env python

#####################################################
# Windows Jenkins script
# Author: Janne Beate Bakeng, SINTEF Medical Technology
# Date:   2013.09.05
#
# Description:
#
#   Continous integration build on Windows: run every time the repo changes.
#   Build part of Jenkins CI
#
#       Download, build, and test CustusX
#       Publish unit tests
#
#
#####################################################

import logging
import time    
import subprocess
import sys
import argparse        

from cx.cxShell import *
import cx.cxInstallData
import cx.cxComponents
import cx.cxComponentAssembly
import cx.cxCustusXBuilder
import cx.cxJenkinsBuildScriptBase

class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBase):
    '''
    Controller that builds, tests and deploys CustusX on a windows machine.
    '''
    def __init__(self):
        ''
        super(Controller, self).__init__()

    def getDescription(self):                  
        return 'Jenkins script for build, test and deployment of CustusX and dependents on a Windows machine.'
       
    def _addArgumentParserArguments(self):
        'subclasses can add parser arguments here'
        super(Controller, self)._addArgumentParserArguments()
        p = self.argumentParser
        p.add_argument('--skip_build', action='store_true', default=False, help='Skip the checkout, configure, build step.')
        p.add_argument('--skip_tests', action='store_true', default=False, help='Skip the test step')
        p.add_argument('--skip_package', action='store_true', default=False, help='Skip creating the installer')
        p.add_argument('--b32', action='store_true', default=False, help='Build 32 bit.')
        p.add_argument('--jom', action='store_true', default=True, help='Use jom to build.')
        p.add_argument('--static', action='store_true', default=True, help='Link statically.')
        p.add_argument('--run_catch_in_ctest', default=None, help='run catch using the input tag string, wrap each test in ctest thus running it as a separate process')

    def _applyArgumentParserArguments(self, options):
        'apply arguments defined in _addArgumentParserArguments()'
        super(Controller, self)._applyArgumentParserArguments(options)
        data = self.cxBuilder.assembly.controlData        
        data.setBuildType("Release")
        if(options.jom):
            data.setCMakeGenerator("NMake Makefiles JOM")
        data.setBuild32(options.b32)
        data.setBuildShared(not options.static)

    def run(self):
        if not self.argumentParserArguments.skip_build:
            self.cxBuilder.buildAllComponents()
        if not self.argumentParserArguments.skip_tests:
            self.cxBuilder.runAllTests()
        if not self.argumentParserArguments.skip_package:
            self.cxBuilder.createInstallerPackage() 
        if self.argumentParserArguments.run_catch_in_ctest is not None:
            print 'running catch in ctest%s' % self.argumentParserArguments.run_catch_in_ctest
            self.cxBuilder.runCatchTestsWrappedInCTest(self.argumentParserArguments.run_catch_in_ctest)
        self.cxBuilder.finish()
                     

if __name__ == '__main__':
    controller = Controller()
    controller.run()