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

class JenkinsBuildScriptBaseBase(object):
    '''
    Base for all scripts working on CustusX
    '''
    def __init__(self):
        ''
        shell.setRedirectOutput(True)
                
        self.cxBuilder = cx.cxCustusXBuilder.CustusXBuilder()
        
        self.additionalParsers = []
        self.addArgParsers()
        self.argumentParser = self._createArgumentParser()
        self.__notused__argumentParserArguments = self.argumentParser.parse_args() # display help etc
        self.applyArgumentParsers()
        
#        self.argumentParserArguments = self.argumentParser.parse_args()
#        self._applyArgumentParserArguments(self.argumentParserArguments)

       
    def run(self):
        raise "Not Implemented"      
    
    def getDescription(self):                  
        return 'Jenkins script for operating on CustusX'
    
    def addArgParsers(self):
        'subclasses can add argparse instances to self.additionalparsers here'
#        p = self.argumentParser
        self.additionalParsers.append(shell.getArgParser())
        self.additionalParsers.append(self.cxBuilder.assembly.controlData.getArgParser_root_dir())
        
        
#        p = argparse.ArgumentParser(add_help=False)
#        p.add_argument('--root_dir', default=None, help='specify root folder, default=None')
 #       self.additionalParsers.append(p)
        #p.add_argument('-d', '--dummy', action='store_true', default=False, help='execute script without calling any shell commands')
        #p.add_argument('--skip_redirect', action='store_true', default=False, help='skip redirecting stout/stderr through python. This can cause stout mangling on the Jenkins server.')
#        pass

    def _createArgumentParser(self):        
        return argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, 
                                       description=self.getDescription(),
                                       parents=self.additionalParsers)

    def applyArgumentParsers(self):
        'apply arguments defined in _addArgumentParserArguments()'
#        shell.setDummyMode(options.dummy)
#        data = self.cxBuilder.assembly.controlData        
#        data.setRootDir(options.root_dir)
        self.cxBuilder.assembly.controlData.applyCommandLine()
#        shell.setRedirectOutput(not options.skip_redirect)
#        print 'PRE', vars(shell)
        shell.applyCommandLine()
#        shell.getArgParser().parse_known_args(namespace=shell)
#        print 'POST', vars(shell)
#        raise "stopped"
    def controlData(self):
        return self.cxBuilder.assembly.controlData

class JenkinsBuildScriptBase_DO_NOT_USE(JenkinsBuildScriptBaseBase):
    '''
    Base for all scripts working on the full build of all components
    '''
    def __init__(self):
        super(JenkinsBuildScriptBase, self).__init__()
           
    def getDescription(self):                  
        return 'Jenkins script for build, test and deployment of CustusX and dependents.'
    
    def addArgParsers(self):
        'subclasses can add argparse instances to self.additionalparsers here'
        self.controlData().setBuildType("Release")
        
        super(JenkinsBuildScriptBase, self).addArgParsers()
        self.additionalParsers.append(self.controlData().getArgParser_core_build())

#    def _addArgumentParserArguments(self):
#        'subclasses can add parser arguments here'
#        super(JenkinsBuildScriptBase, self)._addArgumentParserArguments()
#        p = self.argumentParser
#        p.add_argument('-i', '--isb_password', default="not set", help='password for ISB GE Connection module')
#        p.add_argument('-j', '--threads', type=int, default=1, help='number of make threads')
#        p.add_argument('-g', '--git_tag', default=None, help='git tag to use when checking out CustusX. None means checkout master branch.')
#        pass

    def applyArgumentParsers(self):
#    def _applyArgumentParserArguments(self, options):
#        'apply arguments defined in _addArgumentParserArguments()'
        super(JenkinsBuildScriptBase, self).applyArgumentParsers()
#        data = self.cxBuilder.assembly.controlData        
#        data.mISBpassword = options.isb_password
#        data.threads = options.threads
#        data.mGitTag = options.git_tag
        self.cxBuilder.assembly.controlData.applyCommandLine() # not necessary - done by super
        
