#!/usr/bin/env python

#####################################################
# CustusX setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.16
#
#####################################################

from __future__ import print_function
from builtins import object
import logging
import time    
import subprocess
import sys
import argparse   
import pprint     

from cx.utils.cxShell import *
import cx.build.cxInstallData
import cx.build.cxComponents
import cx.build.cxComponentAssembly
#import cx.cxCustusXBuilder

class BuildScript(object):
    '''
    Base for all scripts working on CustusX
    '''
    def __init__(self, assembly=None):
        ''                
        #self.cxBuilder = cx.cxCustusXBuilder.CustusXBuilder()
        if assembly:
            self.assembly = assembly
        else:
            self.assembly = cx.build.cxComponentAssembly.LibraryAssembly()

        self.setDefaults()
        
        self.additionalParsers = []
        self.addArgParsers()
        self.argumentParser = self._createArgumentParser()
        self.__notused__argumentParserArguments = self.argumentParser.parse_args() # display help etc
        arguments = self.applyArgumentParsers(sys.argv[1:])
        if len(arguments)>0:
            print('Unused command line arguments: ', arguments)
        
        #pprint.pprint('All options: ')
        #pprint.pprint(vars(self.__notused__argumentParserArguments))

       
    def run(self):
        raise "Not Implemented"      
    
    def getDescription(self):                  
        return 'Jenkins script for operating on CustusX'
    
    def setDefaults(self):
        'subclasses can set default values to shell and controlData() here'                
        pass

    def addArgParsers(self):
        'subclasses can add argparse instances to self.additionalparsers here'
        self.additionalParsers.append(shell.getArgParser())

    def _createArgumentParser(self):        
        return argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, 
                                       description=self.getDescription(),
                                       parents=self.additionalParsers)

    def applyArgumentParsers(self, arguments):
        'apply arguments defined in _addArgumentParserArguments()'
        arguments = self.controlData().applyCommandLine(arguments)
        arguments = shell.applyCommandLine(arguments)
        return arguments

    def controlData(self):
        return self.assembly.controlData

