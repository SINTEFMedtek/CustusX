#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#
# Description:
#
#             
#################################################             

import subprocess
import optparse
import re
import sys
import os.path
import urllib
import getpass
import platform

from cxShell import *    
import cxCppBuilder

class Component(object):
    '''
    Data for one Component
    Superclass for all specific components,
    which must implement this interface.
    '''
    def __init__(self):
        pass
    def setControlData(self, data):
        self.controlData = data
    def _createSibling(self, component):
        'create another component of the given type, containing the same control data.'
        retval = component()
        retval.setControlData(self.controlData)
        return retval
    def name(self):
        'return name of component'
        raise "Not Implemented"
    def help(self):
        'description of item'
        return ''
    def path(self):
        'return path where component will be installed'
        raise "Not Implemented"
    def checkout(self):
        'checkout the component source from external source to this computer (svn co or similar)'
        path = self.path()+'/'+self.sourceFolder()
        if os.path.exists(path) and len(os.listdir(path))!=0:
            print "*** %s already exists, checkout ignored." % self.name()
        else:
            self._rawCheckout()
    def _rawCheckout(self):
        'checkout the component source from external source to this computer (svn co or similar)'
        raise "Not Implemented"
    def update(self):
        'update the component source (svn up or similar)'
        pass
    def configure(self):
        'configure build of components (cmake or similar)'
        pass
    def reset(self):
        'delete build folder(s)'
        pass
    def build(self):
        'build and install component (make and possibly make install)'
        pass
    def makeClean(self):
        'make clean'
        pass
    def buildConfigurations(self):
        return []
    def _getBuilder(self):
        builder = cxCppBuilder.CppBuilder()
        builder.setPaths(base=self.path(), build=self.buildPath(), source=self.sourcePath())
        builder.setBuildType(self.getBuildType())
        builder.setControlData(self.controlData)
        return builder
        
# ---------------------------------------------------------

class CppComponent(Component):
    '''
    Implementation of Cpp-style components, i.e all c++ libraries
    that contains a source and build folder, and are built with make.
    Inherit from this to get some methods for free.
    '''
    def __init__(self):
        pass
    def sourceFolder(self):
        return self.name()
    def buildFolder(self):
        return self.controlData.getBuildFolder()
    def _changeDirTo(self, folder):
        changeDir(self.path()+'/'+folder)
    def _changeDirToBase(self):
        changeDir(self.path())
    def _changeDirToSource(self):
        changeDir(self.sourcePath())
    def _changeDirToBuild(self):
        changeDir(self.buildPath())
    def configPath(self):
        return self.buildPath()
    def _configPathFor(self, type):
        'return config path for the input component type'
        return self._createSibling(type).configPath()
    def buildPath(self):
        return self.path()+'/'+self.buildFolder()
    def sourcePath(self):
        return self.path()+'/'+self.sourceFolder()
    def installPath(self):
        return self.buildPath()
    def _rawCheckout(self):
        raise "Not Implemented"
    def configure(self):
        raise "Not Implemented"
    def reset(self):
        'delete build folder(s)'
        self._getBuilder().reset()
    def build(self):
        self._getBuilder().build()
    def makeClean(self):
        self._getBuilder().makeClean()
    def getBuildType(self):
        return self.controlData.getBuildType()
# ---------------------------------------------------------

class ITK(CppComponent):
    def name(self):
        return "ITK"
    def help(self):
        return 'itk.org'
    def path(self):
        return self.controlData.getExternalPath() + "/ITK"
    def sourceFolder(self):
        return 'ITK'
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
        self._getBuilder().gitClone('git://itk.org/ITK.git')
    def update(self):
        self._getBuilder().gitCheckout('v4.1.0')
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('BUILD_TESTING:BOOL', self.controlData.mBuildExAndTest)
        add('BUILD_EXAMPLES:BOOL', self.controlData.mBuildExAndTest)
        builder.configureCMake()
# ---------------------------------------------------------

class VTK(CppComponent):
    def name(self):
        return "VTK"
    def help(self):
        return 'vtk.org'
    def path(self):
        return self.controlData.getExternalPath() + "/VTK"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
        self._getBuilder().gitClone('http://vtk.org/VTK.git')
    def update(self):
        self._getBuilder().gitCheckout('v5.8.0', patch='VTK-5-8-0.patch')
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('VTK_USE_PARALLEL:BOOL', True)
        # Note: DVTK_REQUIRED_OBJCXX_FLAGS is required on v5.6 in order to avoid garbage-collection (!)
        add('VTK_REQUIRED_OBJCXX_FLAGS:STRING', "")
        add('VTK_USE_GUISUPPORT:BOOL', True)
        add('VTK_USE_QT:BOOL', True)
        add('VTK_USE_QVTK:BOOL', True)
        add('VTK_USE_RPATH:BOOL', True)
        add('DESIRED_QT_VERSION:STRING', 4)
        add('BUILD_TESTING:BOOL', self.controlData.mBuildExAndTest)
        add('BUILD_EXAMPLES:BOOL', self.controlData.mBuildExAndTest)
        builder.configureCMake()
# ---------------------------------------------------------

class OpenCV(CppComponent):
    def name(self):
        return "OpenCV"
    def help(self):
        return 'http://opencv.willowgarage.com'
#    def sourceFolder(self):
#        return self.name()+"/opencv"
    def path(self):
        return self.controlData.getExternalPath() + "/OpenCV"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
        self._getBuilder().gitClone('git://code.opencv.org/opencv.git', 'OpenCV')
    def update(self):
        self._getBuilder().gitCheckout('2.4.2')
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('BUILD_TESTING:BOOL', False)
        add('BUILD_EXAMPLES:BOOL', False)
        add('WITH_CUDA:BOOL', False)
        add('BUILD_WITH_STATIC_CRT:BOOL', False)
        add('BUILD_TIFF:BOOL', False)
        add('WITH_TIFF:BOOL', False)
        add('BUILD_JASPER:BOOL', False)
        add('WITH_JASPER:BOOL', False)
        builder.configureCMake()
# ---------------------------------------------------------


class OpenIGTLink(CppComponent):
    def name(self):
        return "OpenIGTLink"
    def help(self):
        return 'http://www.na-mic.org/Wiki/index.php/OpenIGTLink/Library/Build'
    def path(self):
        return self.controlData.getExternalPath() + "/OpenIGTLink"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
        self._getBuilder().gitClone('git://github.com/openigtlink/OpenIGTLink.git')
    def update(self):
        self._getBuilder().gitUpdate('master')
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('BUILD_TESTING:BOOL', False)
        add('BUILD_EXAMPLES:BOOL', False)
        builder.configureCMake()
# ---------------------------------------------------------


class IGSTK(CppComponent):
    def name(self):
        return "IGSTK"
    def help(self):
        return 'igstk.org'
    def path(self):
        return self.controlData.getExternalPath() + "/IGSTK"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
#        self._getBuilder().gitCheckout('git://igstk.org/IGSTK.git')
        self._getBuilder().gitClone('git://igstk.org/IGSTK.git')
    def update(self):
        self._getBuilder().gitCheckout('v5.0', patch='IGSTK-5-0-v2.patch')
    def configure(self):        
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('IGSTK_USE_SceneGraphVisualization:BOOL', False)
        add('ITK_DIR:PATH', self._createSibling(ITK).configPath())
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('IGSTK_SERIAL_PORT_0', self._getSerialPort()) 
        add('BUILD_TESTING:BOOL', False)
        add('BUILD_EXAMPLES:BOOL', False)
        builder.configureCMake()
    def _getSerialPort(self):
        serialPort = "/Library/CustusX/igstk.links/cu.CustusX.dev0"
        if (platform.system() == 'Windows'):
            serialPort = "COM9"
        return serialPort
# ---------------------------------------------------------

#===============================================================================
# class DCMTK(CppComponent):
#    '''
#    The only way to get 3.6.1 to work is to install it,
#    thats the reason for the special treatment in this class.
#    
#    This might change in the future.
#    '''
#    def name(self):
#        return "DCMTK"
#    #def buildFolder(self):
#        #'in-source build because this is necessary for including an uninstalled DCMTK'
#        #return self.sourceFolder()    
#    def help(self):
#        return 'dcmtk.org'
#    def path(self):
#        return self.controlData.mExternalDir + "/DCMTK"
# 
#    def _rawCheckout(self):
#        self._changeDirToBase()
#        runShell('git clone git://git.dcmtk.org/dcmtk DCMTK')
#        # the commontk version of DCMTK compiles without problems on Mac.
#        #runShell('git clone git://github.com/commontk/DCMTK.git DCMTK')     
#        self.update()
#    def update(self):
#        self._changeDirToSource()
#        #runShell('git pull')
#        #runShell('git checkout master')   
#        runShell('git checkout PUBLIC_360')  # 3.6.0 seems to have some issues on fedora 16.  
# 
#    def configure(self):
#        self._changeDirToBuild()
#        runShell('''\
# \cmake \
# -G"%s" \
# -DBUILD_SHARED_LIBS:BOOL=%s \
# -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING="%s" \
# ../%s''' % (self.controlData.mCMakeGenerator,
#            self.controlData.mBuildShared, 
#            self.controlData.mOSX_DEPLOYMENT_TARGET,
#            self.sourceFolder()))
#    def build(self):
#        CppComponent.build(self)
#        runShell('sudo make install')
#    def installPath(self):
#        #return '/usr/local/include/dcmtk'
#        return '' # ignore: use yum instead
#===============================================================================
# ---------------------------------------------------------

class ISB_DataStreaming(CppComponent):
    def name(self):
        self.mCurrentRevision = "444"
        return "ISB_DataStreaming"
    def help(self):
        return 'ISB GE Digital Interface stuff'
    def configPath(self):
        return self.buildPath() + "/vtkDataStreamClient/"
    def path(self):
        return self.controlData.getWorkingPath() + "/ISB_DataStreaming"
        #    def sourceFolder(self):
#return self.name() + "/vtkDataStreamClient/"
        #    def buildFolder(self):
#return self.controlData.getBuildFolder()
    def _rawCheckout(self):
        self._changeDirToBase()
        runShell('svn co http://svn.isb.medisin.ntnu.no/DataStreaming/ -r%s %s %s' % (self.mCurrentRevision, self._svn_login_info(), self.sourceFolder()))
    def update(self):
        self._changeDirToSource()
        runShell('svn up -r%s %s' % (self.mCurrentRevision, self._svn_login_info()))
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('DATASTREAMING_USE_HDF:BOOL', False)
        add('DATASTREAMING_USE_TRACKING:BOOL', False)
        add('DATASTREAMING_USE_SC_DICOM_LOADERS:BOOL', False)
        add('DATASTREAMING_USE_OPENCL:BOOL', True)
        add('BUILD_TESTING:BOOL', self.controlData.mBuildTesting);
        builder.configureCMake()
    def _svn_login_info(self):
        '''
        return login info to be added as arguments to the svn co and up calls.
        '''
        if self.controlData.mISBpassword == "":
            return '--username sintef'
        else:
            return '--non-interactive --username sintef --password %s' % self.controlData.mISBpassword
    # ---------------------------------------------------------

class CustusX3(CppComponent):
    def name(self):
        return "CustusX3"
    def help(self):
        return 'CustusX from SINTEF'
    def path(self):
        return self.controlData.getWorkingPath() + "/CustusX3"    
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:SINTEFMedisinskTeknologi/CustusX3.git')
    def update(self):
        self._getBuilder().gitUpdate(branch='master', submodules=True)    
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        # dependencies
        add('ITK_DIR:PATH', self._createSibling(ITK).configPath())
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('IGSTK_DIR:PATH', self._createSibling(IGSTK).configPath())
        add('OpenIGTLink_DIR:PATH', self._createSibling(OpenIGTLink).configPath())
        add('OpenCV_DIR:PATH', self._createSibling(OpenCV).configPath())
        add('ULTERIUS_INCLUDE_DIR:PATH', self._createSibling(UltrasonixSDK).configPath())
        add('ULTERIUS_LIBRARY:FILEPATH', self._createSibling(UltrasonixSDK).configPath())
        add('Tube-Segmentation-Framework_DIR:PATH', self._createSibling(TubeSegmentationFramework).configPath())
        add('GEStreamer_DIR:PATH', self._createSibling(ISB_DataStreaming).configPath())
        # other options
        add('BUILD_DOCUMENTATION:BOOL', self.controlData.mDoxygen)            
        add('BUILD_OPEN_IGTLINK_SERVER:BOOL', True);
        add('CX_USE_TSF:BOOL', True);
        add('CX_USE_ISB_GE:BOOL', True);
        add('SSC_USE_DCMTK:BOOL', False);
        add('SSC_BUILD_EXAMPLES:BOOL', self.controlData.mBuildSSCExamples);
        add('BUILD_TESTING:BOOL', self.controlData.mBuildTesting);
        add('COTIRE_ADD_UNITY_BUILDS:BOOL', self.controlData.mUseCotire);
        add('COTIRE_ENABLE_PRECOMPILED_HEADERS:BOOL', self.controlData.mUseCotire);
        add('SSC_USE_GCOV:BOOL', self.controlData.mCoverage);
        builder.configureCMake()
# ---------------------------------------------------------

class UltrasonixSDK(CppComponent):
    def name(self):
        return "UltrasonixSDK"
    def help(self):
        return 'UltrasonixSDK'
    def includePath(self):
        return self.path() + "/" + self.sourceFolder() + "/ulterius/inc"
    def libFile(self):
        return self.path() + "/" + self.sourceFolder() + "/ulterius/lib/ulterius.lib"
    def path(self):
        return self.controlData.getWorkingPath() + "/UltrasonixSDK"
    def _rawCheckout(self):
        self._getBuilder().gitClone('ssh://medtek.sintef.no/git/UltrasonixSDK.git')
    def update(self):
        self._getBuilder().gitCheckout('v5.7.4')    
    def configure(self):
        pass
    def build(self):
        pass
# ---------------------------------------------------------

class TubeSegmentationFramework(CppComponent):
    def name(self):
        return "Tube-Segmentation-Framework"
    def help(self):
        return 'Tube-Segmentation-Framework'
    def path(self):
        return self.controlData.getWorkingPath() + "/Tube-Segmentation-Framework"
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:SINTEFMedisinskTeknologi/Tube-Segmentation-Framework.git')
    def update(self):
        self._getBuilder().gitUpdate('master', submodules=True)    
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('USE_C++11', False)
        add('SIPL_USE_GTK', False)
        builder.configureCMake()
        
# ---------------------------------------------------------

class CustusX3Data(CppComponent):
    def name(self):
        return "CustusX3-Data"
    def help(self):
        return 'data files for CustusX'
    def path(self):
        custusx = self._createSibling(CustusX3)
        return custusx.path() + "/" + custusx.sourceFolder()
    def sourceFolder(self):
        return 'data'
    def _rawCheckout(self):
        self._getBuilder().gitClone('ssh://%s'%self.gitRepository(), self.sourceFolder())
    def update(self):
        self._getBuilder().gitUpdate('master')    
    def configure(self):
        pass
    def build(self):
        pass
    def gitRepository(self):
        return 'medtek.sintef.no//Volumes/medtek_HD/git/Data.git'
# ---------------------------------------------------------



