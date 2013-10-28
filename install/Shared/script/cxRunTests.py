#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.10.11
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
from cx.cxPrintFormatter import PrintFormatter

class Controller(cx.cxBuildScript.BuildScript):
    '''
    '''
    def getDescription(self):                  
        return 'Run CustusX tests, using mainly the catch and ctest frameworks.'
       
    def addArgParsers(self):
        self.controlData().setBuildType("Release")
        shell.setRedirectOutput(False)

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

        p.add_argument('--run_default_unit_tests', action='store_true', help='Run the unit tests the default way')
        #p.add_argument('--run_default_integration_tests', action='store_true', help='Run the integration tests the default way')

        p.add_argument('--clear_old_data', action='store_true', help='Reset data repository and delete old test result files.')
        p.add_argument('--run_catch', action='store_true', help='Run catch using the selected options')
        p.add_argument('--wrap_in_ctest', action='store_true', help='Wrap each test in ctest, thus running them as separate processes')
        p.add_argument('catch_arguments', default=None, nargs='*', help='Argument list passed directly to catch')

        return p
    
    def wrapInHyphens(self, text):
        if ' ' in text:
            return '\"%s\"' % text
        else:
            return text
    
    def formatArgumentList(self, args):
        args = [self.wrapInHyphens(x) for x in args]
        args = ' '.join(args)
        return args

    def run(self):
        options = self.options
        
        if options.run_default_unit_tests:
            self.cxBuilder.runUnitTests()
        #if options.run_default_integration_tests:
        #    self.cxBuilder.runAllTests()
        if options.clear_old_data:
            self.cxBuilder.clearTestData()
            
        if options.run_catch:
            args = self.formatArgumentList(options.catch_arguments)
            #PrintFormatter.printInfo('catch arguments: %s' % args)
            if options.wrap_in_ctest:
                self.cxBuilder.runCatchTestsWrappedInCTest(args)                
            else:
                self.cxBuilder.runCatchTests(args)
        self.cxBuilder.finish()
                     

if __name__ == '__main__':
    controller = Controller()
    controller.run()
