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
import cx.cxBuildScript

class Controller(cx.cxBuildScript.BuildScript):
    '''
    '''
    def __init__(self):
        ''
        super(Controller, self).__init__()

    def getDescription(self):                  
        return 'Jenkins script for build, test and deployment of CustusX and dependents.'
       
    def addArgParsers(self):
        self.controlData().setBuildType("Release")
        shell.setRedirectOutput(True)

        super(Controller, self).addArgParsers()
        self.additionalParsers.append(self.controlData().getArgParser_core_build())
        self.additionalParsers.append(self.getArgParser())
       
    def applyArgumentParsers(self, arguments):
        arguments = super(Controller, self).applyArgumentParsers(arguments)
        (self.options, arguments) = self.getArgParser().parse_known_args(arguments)
        print 'Options: ', self.options
        return arguments

    def getArgParser(self):
        p = argparse.ArgumentParser(add_help=False)
        p.add_argument('--skip_build', action='store_true', default=False, help='Skip the checkout, configure, build step.')
        p.add_argument('--skip_tests', action='store_true', default=False, help='Skip the test step')
        p.add_argument('--skip_package', action='store_true', default=False, help='Skip creating the installer')        
        return p

    def run(self):
        options = self.options        
        if not options.skip_build:
            self.cxBuilder.buildAllComponents()
        if not options.skip_tests:
            self.cxBuilder.runUnitTests()
        if not options.skip_package:
            self.cxBuilder.createInstallerPackage()   
        self.cxBuilder.finish()
                     

if __name__ == '__main__':
    controller = Controller()
    controller.run()
