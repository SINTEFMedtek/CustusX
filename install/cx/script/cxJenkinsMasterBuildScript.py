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

import sys
import argparse        
import os

import cxJenkinsBuildScriptBase



class Controller(cxJenkinsBuildScriptBase.Controller):
    '''
    '''
    def __init__(self, assembly=None):
        ''                
        os.environ["MACOSX_DEPLOYMENT_TARGET"] = "10.11" #Compile for macOS El Capitan and above
        super(Controller, self).__init__(assembly)

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
        runs.add_argument('--create_package', action='store_true', default=False, help='run checkout/configure/build, package')
        runs.add_argument('--test_package', action='store_true', default=False, help='install package, run tests: unit, installation, integration')
        runs.add_argument('--create_unit_tested_package', action='store_true', default=False, help='run checkout/configure/build, unit test, package')
        runs.add_argument('--integration_test_package', action='store_true', default=False, help='install package, installation tests, integration tests')
        runs.add_argument('--unstable_test_package', action='store_true', default=False, help='install package, unstable tests')
        runs.add_argument('--analyze', action='store_true', default=False, help='Build and run tests with coverage and analyze tools')

        runs.add_argument('--create_tagged_release', action='store_true', default=False, help='create release folder, publish package and docs to server using git tag')
        runs.add_argument('--create_nightly_release', action='store_true', default=False, help='create release folder, publish package and docs to server using nightly tag')

        skips = p.add_argument_group('Skip options')
        skips.add_argument('--skip_build', action='store_true', default=False, help='Skip the checkout/configure/build step.')
        skips.add_argument('--skip_unit_tests', action='store_true', default=False, help='Skip the unit test step')
        skips.add_argument('--skip_package', action='store_true', default=False, help='Skip creating the installer')        
        
        skips.add_argument('--skip_extra_install_step_checkout', action='store_true', default=False, help='Skip the checkout of code+data prior to installation')
        skips.add_argument('--skip_install', action='store_true', default=False, help='Skip installing the package')
        skips.add_argument('--skip_installation_test', action='store_true', default=False, help='Skip testing the installed package')
        skips.add_argument('--skip_integration_test', action='store_true', default=False, help='Skip integration testing the installed package')

        skips.add_argument('--skip_publish_exe', action='store_true', default=False, help='Skip the publish_(tagged|nightly)_release:publish_exe step')
        skips.add_argument('--skip_publish_docs', action='store_true', default=False, help='Skip the publish_(tagged|nightly)_release:publish_docs step')        
        
        return p
 
    def run(self):
        options = self.options        

        if options.analyze:
            self.runAnalyze()
        if options.reset_installer:
            self.resetInstallerStep()
        if options.create_unit_tested_package:
            self.createUnitTestedPackageStep(skip_build=options.skip_build,  
                                             skip_unit_tests=options.skip_unit_tests,
                                             skip_package=options.skip_package)
        if options.create_package:
            self.createUnitTestedPackageStep(skip_build=options.skip_build,
                                             skip_unit_tests=True,
                                             skip_package=options.skip_package)
        if options.test_package:
            self.testPackageStep(skip_extra_install_step_checkout=options.skip_extra_install_step_checkout,
                                 skip_install=options.skip_install,
                                 skip_unit_tests=options.skip_unit_tests,
                                 skip_installation_test=options.skip_installation_test,            
                                 skip_integration_test=options.skip_integration_test)
        if options.integration_test_package:
            self.integrationTestPackageStep(skip_extra_install_step_checkout=options.skip_extra_install_step_checkout,
                                            skip_install=options.skip_install,
                                            skip_installation_test=options.skip_installation_test,            
                                            skip_integration_test=options.skip_integration_test)
        if options.unstable_test_package:
            self.unstableTestPackageStep(skip_extra_install_step_checkout=options.skip_extra_install_step_checkout,
                                         skip_install=options.skip_install)
        if options.create_tagged_release:
            self.publishTaggedReleaseStep(skip_publish_exe=options.skip_publish_exe, 
                                          skip_publish_docs=options.skip_publish_docs)
        if options.create_nightly_release:
            self.publishNightlyReleaseStep(skip_publish_exe=options.skip_publish_exe, 
                                           skip_publish_docs=options.skip_publish_docs)
                        
        self.cxBuilder.finish()
        
if __name__ == '__main__':
    controller = Controller()
    controller.run()
