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

class Controller(object):
    '''
    '''
    def __init__(self):
        ''
        self.cxBuilder = cx.cxCustusXBuilder.CustusXBuilder()
        self.optionParser = self._createOptionParser()

    def _createOptionParser(self):
        commonOptions = self.cxBuilder.createArgumentParser()
        description='Jenkins script for build, test and deployment of CustusX and dependents.'
        p = argparse.ArgumentParser(description=description, parents=[commonOptions], conflict_handler='resolve')
        p.add_argument('--coverage', action='store_true', default=False, help='gcov code coverage')
        p.add_argument('--doxygen', action='store_true', default=False, help='build doxygen documentation')
        return p
    
    def run(self):
        options = self.optionParser.parse_args()
        shell.setDummyMode(options.dummy)
        self._fillControlDataFromOptions(options)

        self.cxBuilder.buildAllComponents()
        self.cxBuilder.clearTestData()
        self.cxBuilder.runAllTests()
        self.cxBuilder.createInstallerPackage()
                        
    def _fillControlDataFromOptions(self, options):
        data = self.cxBuilder.assembly.controlData
        print "*"*100
        print options
        print "*"*100
        
        data.mRootDir = options.root_dir
        data.setBuildType("Release")
        #data.setBuildType("Debug")
        if options.coverage:
            data.setBuildType("Debug")
        
#        data.mISBpassword = options.isb_password
        data.mCoverage = options.coverage
        data.mDoxygen = options.doxygen
        data.threads = options.threads

if __name__ == '__main__':
    controller = Controller()
    controller.run()
