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
from cxPrintFormatter import PrintFormatter

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
    def isPubliclyAvailable(self):
        return True
    def pluginPath(self):
        'if this component contains a plugin: return path to plugin, absolute or relative to CustusX/source'
        return None
        
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
        self._getBuilder().gitCheckout('v4.5.1')
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
        self._getBuilder().gitClone('git@github.com:SINTEFMedisinskTeknologi/VTK')
    def update(self):
        # this fix should rebase repo from the original Kitware/VTK to our own fork on GitHub.
        repo = 'git@github.com:SINTEFMedisinskTeknologi/VTK'
        branch = 'VTK-CX-modifications'
        tag = 'VTK-6-1-0.cx_patch_1'
        self._getBuilder().gitSetRemoteURL(repo, branch=branch)
        self._getBuilder().gitCheckout(tag)
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('VTK_USE_PARALLEL:BOOL', True)
        # Note: DVTK_REQUIRED_OBJCXX_FLAGS is required on v5.6 in order to avoid garbage-collection (!)
        add('VTK_REQUIRED_OBJCXX_FLAGS:STRING', "")
        #add('VTK_USE_GUISUPPORT:BOOL', True)
        #add('VTK_USE_QT:BOOL', True)
        #add('VTK_USE_QVTK:BOOL', True)
        add('VTK_USE_RPATH:BOOL', True)
        add('DESIRED_QT_VERSION:STRING', 4)
        add('BUILD_TESTING:BOOL', self.controlData.mBuildExAndTest)
        add('BUILD_EXAMPLES:BOOL', self.controlData.mBuildExAndTest)
        add('Module_vtkGUISupportQt:BOOL', True)
        builder.configureCMake()
# ---------------------------------------------------------

class CTK(CppComponent):
    def name(self):
        return "CTK"
    def help(self):
        return 'commontk.org'
    def path(self):
        return self.controlData.getExternalPath() + "/CTK"
#    def buildFolder(self):
#        return "%s/CTK-build" % self.controlData.getBuildFolder()
#    def configPath(self):
#        return self.buildPath() + "/CTK-build/"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:commontk/CTK')
    def update(self):
        latestTestedSHA = '3fe3cdbe9d0ef95b3810a12484f035ae1f66524c'
        self._getBuilder().gitCheckout(latestTestedSHA)
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('CTK_ENABLE_DICOM:BOOL', True)
        add('CTK_LIB_DICOM/Widgets:BOOL', True)
        add('CTK_ENABLE_PluginFramework:BOOL', True)
        add('CTK_BUILD_SHARED_LIBS:BOOL', self.controlData.getBuildShared())
        builder.configureCMake()
        PrintFormatter.printInfo('Build CTK during configure step, in order to create CTKConfig.cmake')
        self.build()
# ---------------------------------------------------------

class OpenCV(CppComponent):
    def name(self):
        return "OpenCV"
    def help(self):
        return 'http://opencv.willowgarage.com'
    def path(self):
        return self.controlData.getExternalPath() + "/OpenCV"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
        self._getBuilder().gitClone('https://github.com/Itseez/opencv.git', 'OpenCV')
    def update(self):
        self._getBuilder().gitCheckout('2.4.8')
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
        self._getBuilder().gitClone('git://igstk.org/IGSTK.git')
    def update(self):
        #self._getBuilder().gitCheckout('v5.2', patch='IGSTK-5-2.patch')
        # this fix should rebase repo from the original Kitware/IGSTK to our own fork on GitHub.
        repo = 'git@github.com:SINTEFMedisinskTeknologi/IGSTK'
        branch = 'IGSTK-CX-modifications'
        tag = 'IGSTK-5-2.cx_patch_3-6-0'
        self._getBuilder().gitSetRemoteURL(repo, branch=branch)
        self._getBuilder().gitCheckout(tag)
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

class ISB_DataStreaming(CppComponent):
    def name(self):
        self.mCurrentRevision = "591"
        return "ISB_DataStreaming"
    def help(self):
        return 'ISB GE Digital Interface stuff'
    def configPath(self):
        return self.buildPath() + "/vtkDataStreamClient/"
    def path(self):
        return self.controlData.getWorkingPath() + "/ISB_DataStreaming"
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
        add('DATASTREAMING_USE_OPENCL:BOOL', self.controlData.mGEStreamerUseOpenCL)
        add('DATASTREAMING_BUILD_TESTING:BOOL', self.controlData.mBuildTesting);
        builder.configureCMake()
    def _svn_login_info(self):
        '''
        return login info to be added as arguments to the svn co and up calls.
        '''
        if self.controlData.isb_password == "":
            return '--username sintef'
        else:
            return '--non-interactive --username sintef --password %s' % self.controlData.isb_password
    def isPubliclyAvailable(self):
        return False
        
# ---------------------------------------------------------

class CustusX(CppComponent):
    def name(self):
        return "CustusX"
    def help(self):
        return 'CustusX from SINTEF'
    def path(self):
        return self.controlData.getWorkingPath() + "/CustusX"    
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:SINTEFMedisinskTeknologi/CustusX3.git')
    def update(self):
        self._getBuilder().gitUpdate('master', tag=self.controlData.getGitTag(), submodules=True)    
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('ITK_DIR:PATH', self._createSibling(ITK).configPath())
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('IGSTK_DIR:PATH', self._createSibling(IGSTK).configPath())
        add('OpenIGTLink_DIR:PATH', self._createSibling(OpenIGTLink).configPath())
        add('OpenCV_DIR:PATH', self._createSibling(OpenCV).configPath())
        add('CTK_DIR:PATH', self._createSibling(CTK).configPath())
        add('ULTERIUS_INCLUDE_DIR:PATH', self._createSibling(UltrasonixSDK).includePath())
        add('ULTERIUS_LIBRARY:FILEPATH', self._createSibling(UltrasonixSDK).libFile())
        add('ULTERIUS_BIN_DIR:FILEPATH', self._createSibling(UltrasonixSDK).binDir())
        add('Tube-Segmentation-Framework_DIR:PATH', self._createSibling(TubeSegmentationFramework).configPath())
        add('Level-Set-Segmentation_DIR:PATH', self._createSibling(LevelSetSegmentation).configPath())
        add('OpenCLUtilityLibrary_DIR:PATH', self._createSibling(OpenCLUtilityLibrary).configPath())
        add('GEStreamer_DIR:PATH', self._createSibling(ISB_DataStreaming).configPath())
        add('BUILD_DOCUMENTATION:BOOL', self.controlData.mDoxygen)            
        add('SSC_BUILD_EXAMPLES:BOOL', self.controlData.mBuildSSCExamples);
        add('BUILD_TESTING:BOOL', self.controlData.mBuildTesting);
        add('COTIRE_ADD_UNITY_BUILDS:BOOL', self.controlData.mUseCotire);
        add('COTIRE_ENABLE_PRECOMPILED_HEADERS:BOOL', self.controlData.mUseCotire);
        add('SSC_USE_GCOV:BOOL', self.controlData.mCoverage);
        
        libs = self.assembly.libraries
        for lib in libs:
            if lib.pluginPath():
                add('CX_EXTERNAL_PLUGIN_%s'%lib.name(), lib.pluginPath())
#                add('CX_EXTERNAL_PLUGIN_%s'%lib.name(), '%s:ON'%lib.pluginPath())
        
        if self.controlData.force_connect_sublibraries:
            self.forceConnectSublibraries(add)
        cmakeOptions = ''
        if self.controlData.mGraphviz:
            cmakeOptions = '--graphviz=graph.dot'
        builder.configureCMake(cmakeOptions)
    def forceConnectSublibraries(self, add):
        print 'CustusX force connect sublibraries.'
        add('BUILD_OPEN_IGTLINK_SERVER:BOOL', True);
        add('CX_USE_LEVEL_SET:BOOL', platform.system() == 'Linux')
        add('CX_USE_TSF:BOOL', platform.system() != 'Windows');
        add('CX_USE_ISB_GE:BOOL', platform.system() != 'Windows');
        add('CX_BUILD_MEHDI_VTKMULTIVOLUME:BOOL', False);
        add('CX_BUILD_US_SIMULATOR:BOOL', True);
        
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
    def binDir(self):
        return self.path() + "/" + self.sourceFolder() + "/bin"
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
    def makeClean(self):
        pass
    def isPubliclyAvailable(self):
        return False

# ---------------------------------------------------------

class UltrasoundSimulation(CppComponent):
    def name(self):
        return "UltrasoundSimulation"
    def help(self):
        return 'UltrasoundSimulation'
    def path(self):
        return self.controlData.getWorkingPath() + "/UltrasoundSimulation"
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:SINTEFMedisinskTeknologi/UltrasoundSimulation.git')
    def update(self):
        self._getBuilder().gitCheckout('978f9980781303d626dc390df4f8dd73ecb3b717')
    def configure(self):
        pass
    def build(self):
        pass
    def makeClean(self):
        pass
    def isPubliclyAvailable(self):
        return False

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
        #self._getBuilder().gitCheckout('e83582d2cae965f5a135cfa2b49c5ba68f7cb3f0', submodules=True)
        self._getBuilder().gitUpdate('master', submodules=True)
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('USE_C++11:BOOL', False)
        add('SIPL_USE_GTK:BOOL', False)
        add('sipl_use_gtk:BOOL', False) #variables in cmake are case sensitive, SIPL uses this options
        add('TSF_USE_EXTRNAL_OUL:BOOL', True)
        add('TSF_EXTERNAL_OUL_PATH:PATH', self._createSibling(OpenCLUtilityLibrary).findPackagePath())
        builder.configureCMake()

 # ---------------------------------------------------------

class LevelSetSegmentation(CppComponent):
    def name(self):
        return "Level-Set-Segmentation"
    def help(self):
        return 'Level-Set-Segmentation'
    def path(self):
        return self.controlData.getWorkingPath() + "/Level-Set-Segmentation"
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:smistad/Level-Set-Segmentation')
    def update(self):
        self._getBuilder().gitCheckout('6bf9277ac604ecf6f10c0a70d9e04ae5489c2060', submodules=True)
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('sipl_use_gtk:BOOL', False)
        add('LS_USE_EXTRNAL_OUL:BOOL', True)
        add('LS_EXTERNAL_OUL_PATH:PATH', self._createSibling(OpenCLUtilityLibrary).findPackagePath())
        builder.configureCMake()
        
# ---------------------------------------------------------

class OpenCLUtilityLibrary(CppComponent):
    def name(self):
        return "OpenCLUtilityLibrary"
    def help(self):
        return 'OpenCLUtilityLibrary. Functions for interacting with OpenCL.'
    def path(self):
        return self.controlData.getWorkingPath() + "/OpenCLUtilityLibrary"
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:smistad/OpenCLUtilityLibrary')
    def update(self):
        self._getBuilder().gitCheckout('d3b05fd1539d7d62e6c5f696ca8a31fd98e571b2', submodules=False)
    def configure(self):
        builder = self._getBuilder()
        builder.configureCMake()
    def findPackagePath(self):
        return self.buildPath()
        
# ---------------------------------------------------------

class CustusXData(CppComponent):
    def name(self):
        return "CustusX-Data"
    def help(self):
        return 'data files for CustusX'
    def path(self):
        custusx = self._createSibling(CustusX)
        return custusx.path() + "/" + custusx.sourceFolder()
    def sourceFolder(self):
        return 'data'
    def _rawCheckout(self):
        self._getBuilder().gitClone('ssh://%s'%self.gitRepository(), self.sourceFolder())
    def update(self):
        self._getBuilder().gitUpdate('master', tag=self.controlData.getGitTag())    
    def configure(self):
        pass
    def build(self):
        pass
    def gitRepository(self):
        return 'medtek.sintef.no//Volumes/medtek_HD/git/Data.git'
    def makeClean(self):
        pass
    def isPubliclyAvailable(self):
        return False
# ---------------------------------------------------------


