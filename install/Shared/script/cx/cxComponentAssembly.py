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
        self.libraries = []
        self.libraries.append(cxComponents.ITK())
        self.libraries.append(cxComponents.VTK())
        self.libraries.append(cxComponents.OpenCV())
        self.libraries.append(cxComponents.OpenIGTLink())
        self.libraries.append(cxComponents.IGSTK())
        self.libraries.append(cxComponents.CTK())
        if (platform.system() != 'Windows'):
             self.libraries.append(cxComponents.ISB_DataStreaming());
        self.libraries.append(cxComponents.UltrasonixSDK())
        if (platform.system() != 'Windows'):
            self.libraries.append(cxComponents.OpenCLUtilityLibrary())
        if (platform.system() != 'Windows'):
            self.libraries.append(cxComponents.TubeSegmentationFramework());
        if (platform.system() == 'Linux'):
            self.libraries.append(cxComponents.LevelSetSegmentation());
        self.libraries.append(cxComponents.UltrasoundSimulation())
        self.libraries.append(cxComponents.CustusX3())
        self.libraries.append(cxComponents.CustusX3Data())
        
        for lib in self.libraries:
            lib.setControlData(self.controlData)

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
    
    def selectAllPublicLibraries(self):
        selection = [lib.name() for lib in self.libraries if lib.isPubliclyAvailable()]
        self.selectedLibraryNames = selection

    def process(self, checkout=False, configure_clean=False, configure=False, clean=False, build=False):
        '''
        checkout, configure, build
        '''
        selectedLibraries = [lib for lib in self.libraries if lib.name() in self.selectedLibraryNames]
                    
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
            
            


