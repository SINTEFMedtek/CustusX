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

import sys
import argparse        

import cxJenkinsBuildScriptBase

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
        runs.add_argument('--nightly_I', action='store_true', default=False, help='nightly build, up to test evaluation')
        runs.add_argument('--nightly_II', action='store_true', default=False, help='nightly build, after successful test evaluation')

        return p
 
    def run(self):
        
        self.try_nightly_build()
        # .. add more builds here
                        
        self.cxBuilder.finish()
        
    def is_main_build(self):
        return self.options.target == "ubuntu.12.04.x64"

    def try_nightly_build(self):
        '''
        Run the nightly builds, which does everything from scratch
        and publishes binaries and documentation to the web server.
        '''
        if self.options.nightly_I:
            self.controlData().build_developer_doc = True
            self.controlData().build_user_doc = True
            self.resetInstallerStep()
            self.createUnitTestedPackageStep()
            self.integrationTestPackageStep(skip_extra_install_step_checkout=options.skip_extra_install_step_checkout)
        if self.options.nightly_II:
            self.publishNightlyRelease()
            if self.is_main_build():
                self.publishNightlyDocumentation()
        
