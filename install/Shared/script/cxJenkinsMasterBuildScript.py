#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.10.13
#
# Description:
#
#
#####################################################

import logging
import time    
import subprocess
import sys
import argparse        

from cx.cxShell import *
from cx.cxPrintFormatter import PrintFormatter
import cx.cxInstallData
import cx.cxComponents
import cx.cxComponentAssembly
import cx.cxCustusXBuilder
import cx.cxCustusXInstaller
import cx.cxCustusXTestInstallation

import cx.cxJenkinsBuildScriptBase


class Controller(cx.cxJenkinsBuildScriptBase.JenkinsBuildScriptBase):
    '''
    '''

    def getDescription(self):                  
        return 'Flexible script for running some or all of the operations on the jenkins server.'
           
    def addArgParsers(self):
        super(Controller, self).addArgParsers()
        self.additionalParsers.append(self.getArgParser())

    def applyArgumentParsers(self, arguments):
        arguments = super(Controller, self).applyArgumentParsers(arguments)
        (self.options, arguments) = self.getArgParser().parse_known_args(arguments)
        print 'Master options: ', self.options

        return arguments

    def getArgParser(self):
        p = argparse.ArgumentParser(add_help=False)
        
        runs = p.add_argument_group('Run options')
        runs.add_argument('--reset_installer', action='store_true', default=False, help='Remove installer files and current installation')
        runs.add_argument('--create_unit_tested_package', action='store_true', default=False, help='run checkout/configure/build, unit test, package')
        runs.add_argument('--integration_test_package', action='store_true', default=False, help='install package, installation tests, integration tests')
        runs.add_argument('--unstable_test_package', action='store_true', default=False, help='install package, unstable tests')
        runs.add_argument('--create_release', action='store_true', default=False, help='create release folder, publish to server')

        skips = p.add_argument_group('Skip options')
        skips.add_argument('--skip_build', action='store_true', default=False, help='Skip the checkout/configure/build step.')
        skips.add_argument('--skip_unit_tests', action='store_true', default=False, help='Skip the unit test step')
        skips.add_argument('--skip_package', action='store_true', default=False, help='Skip creating the installer')        
        
        skips.add_argument('--skip_extra_install_step_checkout', action='store_true', default=False, help='Skip the checkout of code+data prior to installation')
        skips.add_argument('--skip_install', action='store_true', default=False, help='Skip installing the package')
        skips.add_argument('--skip_installation_test', action='store_true', default=False, help='Skip testing the installed package')
        skips.add_argument('--skip_integration_test', action='store_true', default=False, help='Skip integration testing the installed package')

        skips.add_argument('--skip_publish_release', action='store_true', default=False, help='Skip the create_release:publish_to_server step')
        
        return p
 
    def run(self):
        #PrintFormatter.printHeader('Building release for tag "%s"' % self.controlData().git_tag, 1);
        options = self.options        

        if options.reset_installer:
            self.resetInstallerStep()
        if options.create_unit_tested_package:
            self.createUnitTestedPackageStep(skip_build=options.skip_build,  
                                             skip_unit_tests=options.skip_unit_tests,
                                             skip_package=options.skip_package)
        if options.integration_test_package:
            self.integrationTestPackageStep(skip_extra_install_step_checkout=options.skip_extra_install_step_checkout,
                                            skip_install=options.skip_install,
                                            skip_installation_test=options.skip_installation_test,            
                                            skip_integration_test=options.skip_integration_test)
        if options.unstable_test_package:
            self.unstableTestPackageStep(skip_extra_install_step_checkout=options.skip_extra_install_step_checkout,
                                            skip_install=options.skip_install)
        if options.create_release:
            self.createReleaseStep(skip_publish_release=options.skip_publish_release)
        self.cxBuilder.finish()
        
if __name__ == '__main__':
    controller = Controller()
    controller.run()
