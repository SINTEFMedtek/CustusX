#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.05.27
#
# Description:
#
#
#################################################             

import subprocess
import argparse
import re
import sys
import os.path
import urllib
import getpass
import platform
import time

from cxShell import *
from cxPrintFormatter import PrintFormatter
import cxInstallData
import cxComponents

class LibraryAssembly:
    '''
    Manages the work of all libraries
    '''
    def __init__(self):
        self.controlData = cxInstallData.Common()
        self.libraries = [
                     #CppUnit(),
                     cxComponents.ITK(),
                     cxComponents.VTK(),
                     cxComponents.OpenCV(),
                     cxComponents.OpenIGTLink(),
                     cxComponents.IGSTK(),
                     #DCMTK(),
                     #SSC(),
                     cxComponents.ISB_DataStreaming(),
                     cxComponents.UltrasonixSDK(),
                     cxComponents.TubeSegmentationFramework(),
                     cxComponents.CustusX3(),
                     cxComponents.CustusX3Data()
                     ]
        for lib in self.libraries:
            lib.setControlData(self.controlData)
#        self._startTime = time.time()

    def getComponent(self, type):
        for comp in self.libraries:
            if isinstance(comp, type):
                return comp
        return None

    def getLibnames(self):
        return [lib.name() for lib in self.libraries]
    
    def selectLibraries(self, selectedLibraryNames):
        all = self.getLibnames()
        filtered = [val for val in all if val in selectedLibraryNames]
        self.selectedLibraryNames = filtered

    def selectAllLibraries(self):
        self.selectedLibraryNames = self.getLibnames()
                        
    def getSelectedLibraries(self):
        return self.selectedLibraryNames

    def process(self, checkout=False, configure_clean=False, configure=False, clean=False, build=False):
        '''
        checkout, configure, build
        '''
        selectedLibraries = [lib for lib in self.libraries if lib.name() in self.selectedLibraryNames]
                    
#        for lib in selectedLibraries:
#            lib.setControlData(self.controlData)

        operations = []

        if checkout:
            operations.append('checkout')
        if checkout:
            operations.append('update')
        if configure_clean:
            operations.append('reset')
        if configure or configure_clean:
            operations.append('configure')
        if clean:
            operations.append('makeClean')
        if build:
            operations.append('build')
            
        for oper in operations:
            self._operation(selectedLibraries, oper)

    def _operation(self, libraries, methodname):
        for lib in libraries:
            #print '\n================== %s %s========================' % (methodname, lib.name())
            #print '\n%s %-*s %s' % ('='*20, 20, methodname + " " + lib.name(), '='*20)
            text = methodname+" "+lib.name()
            PrintFormatter.printHeader(text)
#            print '\n%s %s' % ('='*20, " %s ".ljust(60, '=')%text)
            method = getattr(lib, methodname)
            method()
            
            


