#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2015.02.16
#
# Description:
#
#
#####################################################

from __future__ import absolute_import
import sys
import argparse        

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter
import cx.build.cxInstallData
import cx.build.cxComponents
import cx.build.cxCustusXBuilder

from . import cxJenkinsBuildScriptBase

class Controller(cxJenkinsBuildScriptBase.Controller):
    '''
    '''
    def __init__(self, assembly=None):
        ''                
        super(Controller, self).__init__(assembly)

    def getDescription(self):                  
        return 'Contains all jobs on the Jenkins server: Select job and step using command-line arguments'
           
    def addArgParsers(self):
        super(Controller, self).addArgParsers()
        self.additionalParsers.append(self.getArgParser())

    def applyArgumentParsers(self, arguments):
        arguments = super(Controller, self).applyArgumentParsers(arguments)
        (self.options, arguments) = self.getArgParser().parse_known_args(arguments)
        return arguments

    def getArgParser(self):
        p = argparse.ArgumentParser(add_help=False)

        jobargs = p.add_argument_group('Job Arguments')
        jobargs.add_argument('--target', default=None, metavar='TARGET', dest='target', help='Jenkins build target, i.e. TARGET var. REQUIRED!')
        
        runs = p.add_argument_group('Job Selection')
        # nightly
        runs.add_argument('--job_nightly_I', action='store_true', default=False, help='nightly build, up to test evaluation')
        runs.add_argument('--job_nightly_II', action='store_true', default=False, help='nightly build, after successful test evaluation')
        # analysis
        runs.add_argument('--job_analysis', action='store_true', default=False, help='analysis build, perform static analysis actions')
        # unit
        runs.add_argument('--job_unit', action='store_true', default=False, help='unit test build')
        # integration
        runs.add_argument('--job_integration_I_clean', action='store_true', default=False, help='integration test build, clean at startup')
        runs.add_argument('--job_integration_II_build', action='store_true', default=False, help='integration test build')
        # unstable
        runs.add_argument('--job_unstable_I_clean', action='store_true', default=False, help='unstable test build, clean at startup')
        runs.add_argument('--job_unstable_II_build', action='store_true', default=False, help='unstable test build')
        # release
        runs.add_argument('--job_release_I', action='store_true', default=False, help='release build, up to test evaluation')
        runs.add_argument('--job_release_II', action='store_true', default=False, help='release build, after successful test evaluation')

        return p
 
    def run(self):
        '''
        Try all jobs. Only those with corresponding
        command-line arguments are run.
        '''
        self.try_job_unit_test()
        self.try_job_integration_test()
        self.try_job_unstable_test()
        self.try_job_nightly()
        self.try_job_analysis()
        self.try_job_release()
        # .. add more builds here
                        
        self.cxBuilder.finish()
        
    def is_main_build(self):
        return self.options.target == "ubuntu.12.04.x64"

    def try_job_unit_test(self):
        '''
        Run the unit test job
        '''
        if self.options.job_unit:
            self.createUnitTestedPackageStep()
            self.cxBuilder.finish()                     

    def try_job_integration_test(self):
        '''
        Run the integration test job
        '''
        if self.options.job_integration_I_clean:
            self.resetInstallerStep()
            self.cxBuilder.finish()
        if self.options.job_integration_II_build:
            self.integrationTestPackageStep()
            self.cxBuilder.finish()

    def try_job_unstable_test(self):
        '''
        Run the integration test job
        '''
        if self.options.job_unstable_I_clean:
            self.resetInstallerStep()
            self.cxBuilder.finish()
        if self.options.job_unstable_II_build:
            self.unstableTestPackageStep()
            self.cxBuilder.finish()

    def try_job_nightly(self):
        '''
        Run the nightly builds, which does everything from scratch
        and publishes binaries and documentation to the web server.
        '''
        if self.options.job_nightly_I:
            if self.is_main_build():
                self.controlData().build_developer_doc = True
                self.controlData().build_user_doc = True
            self.resetInstallerStep()
            self.createUnitTestedPackageStep()
            self.integrationTestPackageStep(skip_extra_install_step_checkout=True)
            self.cxBuilder.finish()
        if self.options.job_nightly_II:
            self.publishNightlyRelease()
            if self.is_main_build():
                self.publishNightlyDocumentation()
            self.cxBuilder.finish()
                
    def try_job_analysis(self):
        '''
        Run the nightly builds, which does everything from scratch
        and publishes binaries and documentation to the web server.
        '''
        if self.options.job_analysis:
            self.controlData().setBuildType("Debug")
            self.controlData().mCoverage = True

            # build often fails due to failure to read gcno files in the build folder..
            # solve by clearing contents, this is faster than clearing entire workspace.
            self.cxBuilder.deleteCustusXBuildFolder() 
            self.cxBuilder.buildAllComponents()
            self.cxBuilder.resetCoverage()
            self.cxBuilder.runUnitTests()
            self.cxBuilder.generateCoverageReport()
            self.cxBuilder.runCppCheck()
            self.cxBuilder.runLineCounter()
            self.cxBuilder.publishCoverageInfo(targetFolder = "nightly")        

            self.cxBuilder.finish()

    def try_job_release(self):
        '''
        Create a tagged release and publish to server.
        Build, run all tests, publish binaries and 
        documentation to the web server.
        
        Requires a git_tag as input.
        Assumed to be run on the release branch (should work OK on others as well)
        '''
        if self.options.job_release_I:
            PrintFormatter.printHeader('Building release for tag "%s"' % self.controlData().git_tag, 1);
            if self.is_main_build():
                self.controlData().build_developer_doc = True
                self.controlData().build_user_doc = True
            self.resetInstallerStep()
            self.createUnitTestedPackageStep()
            self.integrationTestPackageStep(skip_extra_install_step_checkout=True)
            self.cxBuilder.finish()
        if self.options.job_release_II:
            targetFolder = self.cxInstaller.getTaggedFolderName()
            PrintFormatter.printHeader('Creating and publishing release for tag "%s"' % self.controlData().git_tag, 1);
            if targetFolder != self.controlData().git_tag:
                PrintFormatter.printInfo("Warning: Target folder [%s] not equal to controldata tag [%s]" % (targetFolder, self.controlData().git_tag) )
            self.publishTaggedRelease()
            if self.is_main_build():
                self.publishTaggedDocumentation()
            self.cxBuilder.finish()
                                
        
