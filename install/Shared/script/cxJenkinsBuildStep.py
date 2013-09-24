#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
# Description:
#
#   Continous integration build: run every time the repo changes.
#   Experimental!
#   Build part of jenkins CI
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
    '''
    def __init__(self):
        ''
        super(Controller, self).__init__()

    def getDescription(self):                  
        return 'Jenkins script for build, test and deployment of CustusX and dependents.'
       
    def _addArgumentParserArguments(self):
        'subclasses can add parser arguments here'
        super(Controller, self)._addArgumentParserArguments()
        p = self.argumentParser
        p.add_argument('--skip_build', action='store_true', default=False, help='Skip the checkout, configure, build step.')
        p.add_argument('--skip_tests', action='store_true', default=False, help='Skip the test step')
        p.add_argument('--skip_package', action='store_true', default=False, help='Skip creating the installer')        

        p.add_argument('--run_catch', default=None, help='run catch using the input tag string')
        p.add_argument('--run_catch_in_ctest', default=None, help='run catch using the input tag string, wrap each test in ctest thus running it as a separate process')        
        p.add_argument('-t', '--build_type', choices=['Debug','Release','RelWithDebInfo'], help='build type', default='Release')

    def _applyArgumentParserArguments(self, options):
        'apply arguments defined in _addArgumentParserArguments()'
        super(Controller, self)._applyArgumentParserArguments(options)
        data = self.cxBuilder.assembly.controlData        
        data.setBuildType(options.build_type)
#        data.setBuildType("Release")

    def run(self):
        options = self.argumentParserArguments
        
        print 'options.run_catch', options.run_catch
        print 'options.run_catch_in_ctest', options.run_catch_in_ctest
        
        if not options.skip_build:
            self.cxBuilder.buildAllComponents()
        if not options.skip_tests:
            #self.cxBuilder.clearTestData()
            self.cxBuilder.runAllTests()
        if options.run_catch is not None:
            print 'running catch %s' % options.run_catch
            self.cxBuilder.runCatchTests(options.run_catch)
        if options.run_catch_in_ctest is not None:
            print 'running catch in ctest%s' % options.run_catch_in_ctest
            self.cxBuilder.runCatchTestsWrappedInCTest(options.run_catch_in_ctest)
        if not options.skip_package:
            self.cxBuilder.createInstallerPackage()   
        self.cxBuilder.finish()
                     

if __name__ == '__main__':
    controller = Controller()
    controller.run()
