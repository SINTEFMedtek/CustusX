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

from __future__ import absolute_import
import subprocess
import argparse
import re
import sys
import os.path
import glob
import shutil
import stat
import urllib.request, urllib.parse, urllib.error
import getpass
import platform
import time

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter
from . import cxInstallData
from . import cxComponents

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
        if self.controlData.mBuildIGSTK:
            self.addComponent(cxComponents.VTK92())
            self.addComponent(cxComponents.ITK4())
            self.addComponent(cxComponents.IGSTK())
        else:
            self.addComponent(cxComponents.VTK())
            self.addComponent(cxComponents.ITK())
        self.addComponent(cxComponents.OpenCV())
        self.addComponent(cxComponents.OpenIGTLink())
        self.addComponent(cxComponents.CTK())
        self.addComponent(cxComponents.OpenIGTLinkIO())
        self.addComponent(cxComponents.OpenCLUtilityLibrary())
        self.addComponent(cxComponents.QHttpServer())
        #if(platform.system() == 'Linux'):
        #  self.addComponent(cxComponents.FAST()) # NB: Using old version of FAST for macOS
        #self.addComponent(cxComponents.org_custusx_angleCorrection())
				#self.addComponent(cxComponents.org_custusx_mariana())
        self.addComponent(self.custusx)
        self.addComponent(cxComponents.CustusXData())
        
    def addComponent(self, component):
        self.libraries.append(component)
        component.setControlData(self.controlData)
        component.assembly = self # why: config in custusx need all other libs

    def removeComponent(self, component):
        '''
        Register a component that USED TO be part of this assembly (call this where
        addComponent(component) used to be called, when removing a component from an
        assembly, instead of just deleting/commenting out that line).

        Why: every external component and external plugin is checked out and built
        into a predictable, name-based location (component.path(), the same location
        addComponent'd components use) on whatever machine runs the build -- a
        developer's own machine reused across many builds over time, or a CI runner
        with a persistent build tree reused across pipeline runs. If a component is
        simply removed from the assembly (its addComponent(...) call deleted/commented
        out), a leftover checkout+build from before the removal is never cleaned up by
        a later build: nothing there is a target of any operation anymore, so it just
        sits there untouched, and can still be picked up (e.g. a stale plugin .so with
        an obsolete dependency being swept into CPack packaging by the generic plugin
        glob). Calling removeComponent() here instead means every build -- a developer
        rebuilding an old checkout, or CI on any runner -- deletes that leftover the
        moment it constructs this same assembly, before doing anything else.
        '''
        component.setControlData(self.controlData)
        if component.pluginPath():
            # Plugin-style component: path() is the *shared* source/plugins folder,
            # not a private top-level one -- only remove this component's own
            # checkout inside it, never the whole shared folder.
            target = component.sourcePath()
        else:
            # Regular external component: path() is this component's own private
            # top-level folder (its source and build dirs alike) -- remove all of it.
            target = component.path()
        if os.path.isdir(target):
            PrintFormatter.printHeader('Removing leftover checkout of removed component %s: %s' % (component.name(), target))
            shutil.rmtree(target, onerror=self._forceRemoveReadonly)
        if component.pluginPath():
            self._removeStalePluginBinaries(target)

    def _removeStalePluginBinaries(self, plugin_source_path):
        'A stale plugin binary would otherwise still be packaged by CPack.'
        custusx = cxComponents.CustusX()
        custusx.setControlData(self.controlData)
        target_name = os.path.basename(os.path.normpath(plugin_source_path)).replace('.', '_')
        bin_path = os.path.join(custusx.buildPath(), 'bin')
        patterns = [os.path.join(bin_path, 'plugins', 'lib%s.*' % target_name),
                    os.path.join(bin_path, '%s.*' % target_name)]
        for pattern in patterns:
            for stale in glob.glob(pattern):
                PrintFormatter.printHeader('Removing leftover plugin binary of removed component: %s' % stale)
                if os.path.isdir(stale) and not os.path.islink(stale):
                    shutil.rmtree(stale, onerror=self._forceRemoveReadonly)
                else:
                    os.remove(stale)

    @staticmethod
    def _forceRemoveReadonly(func, path, exc_info):
        '''
        onerror handler for shutil.rmtree: git leaves loose/packed object
        files inside .git read-only, and shutil.rmtree's os.unlink can't
        delete a read-only file on Windows (unlike Linux/Mac, where directory
        write permission is all that matters). Clear the read-only bit and
        retry the failed operation.
        '''
        os.chmod(path, stat.S_IWRITE)
        func(path)

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
                      
    def selectTestLibraries(self):
        libs = [lib for lib in self.libraries if lib.useInIntegrationTesting()]
        self.selectedLibraryNames = [lib.name() for lib in libs]
                        
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
            
        for lib in selectedLibraries:
            for oper in operations:
                self._operation(lib, oper)


    def _operation(self, lib, methodname):
            #print '\n================== %s %s========================' % (methodname, lib.name())
            #print '\n%s %-*s %s' % ('='*20, 20, methodname + " " + lib.name(), '='*20)
        text = methodname+" "+lib.name()
        PrintFormatter.printHeader(text)
#            print '\n%s %s' % ('='*20, " %s ".ljust(60, '=')%text)
        method = getattr(lib, methodname)
        method()
            
            


