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

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter
import cxInstallData
import cxComponents

class LibraryAssembly(object):
    '''
    Manages the work of all libraries
    '''
    def __init__(self, controlData=None):
        if (controlData==None):
            self.controlData = cxInstallData.Common()
        else:
            self.controlData = controlData
            
        self.libraries = []
        self.custusx = cxComponents.CustusX()

        self.addComponent(cxComponents.Eigen())
        self.addComponent(cxComponents.VTK())
        self.addComponent(cxComponents.ITK())
        self.addComponent(cxComponents.OpenCV())
        self.addComponent(cxComponents.OpenIGTLink())
        self.addComponent(cxComponents.IGSTK())
        self.addComponent(cxComponents.CTK())
        self.addComponent(cxComponents.OpenCLUtilityLibrary())
        self.addComponent(cxComponents.TubeSegmentationFramework());
        if (platform.system() == 'Linux') or (platform.system() == 'Darwin'):
            self.addComponent(cxComponents.LevelSetSegmentation());
        self.addComponent(self.custusx)
        self.addComponent(cxComponents.CustusXData())
        
    def addComponent(self, component):
        self.libraries.append(component)
        component.setControlData(self.controlData)
        component.assembly = self # why: config in custusx need all other libs

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
        checkout, configure, make
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
            
            


