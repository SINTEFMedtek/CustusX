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
    def _checkout_check_exist(self, path):
        if os.path.exists(path):
            print "*** %s already exists, checkout ignored." % path
            return True
        return False
    def checkout(self):
        'checkout the component source from external source to this computer (svn co or similar)'
        path = self.path()+'/'+self.sourceFolder()
        if self._checkout_check_exist('%s/.git'%path):
            return
        if self._checkout_check_exist('%s/.svn'%path):
            return
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
    def addConfigurationToDownstreamLib(self, builder):
        'called during the configuration step of CustusX: insert necessary cmake stuff into builder'
        pass

        
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
        tag = 'VTK-6-1-0.cx_patch_2'
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
#        add('DESIRED_QT_VERSION:STRING', 4)
        
        use_qt5 = True
        if use_qt5:
#            qt5_root = '/Users/christiana/Qt/5.3/clang_64'
            add('VTK_QT_VERSION:STRING', "5")
#            add('QT_QMAKE_EXECUTABLE:PATH', "%s/bin/qmake"%qt5_root)
            add('VTK_Group_Qt:BOOL', "ON")
#            add('CMAKE_PREFIX_PATH:PATH', "%s/lib/cmake"%qt5_root)
        else:
            add('DESIRED_QT_VERSION:STRING', 4)
            add('Module_vtkGUISupportQt:BOOL', True)
            add('VTK_USE_PARALLEL:BOOL', True)
            add('VTK_USE_RPATH:BOOL', True)        
        
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
        self._getBuilder().gitClone('git@github.com:SINTEFMedisinskTeknologi/CTK.git')
    def update(self):
        #latestTestedSHA = '3fe3cdbe9d0ef95b3810a12484f035ae1f66524c'
        modBranch = 'CTK-CX-modifications'
        self._getBuilder().gitUpdate(modBranch)
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('CTK_QT_VERSION:STRING', 5)
        add('CTK_ENABLE_DICOM:BOOL', True)
        add('CTK_LIB_DICOM/Widgets:BOOL', True)
        add('CTK_ENABLE_PluginFramework:BOOL', True)
        #add('CTK_BUILD_SHARED_LIBS:BOOL', self.controlData.getBuildShared())
        add('CTK_BUILD_SHARED_LIBS:BOOL', True)
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

class Eigen(CppComponent):
    def name(self):
        return "eigen"
    def help(self):
        return 'http://eigen.tuxfamily.org/'
    def path(self):
        return self.controlData.getExternalPath() + "/eigen"
    def configPath(self):
        return self.sourcePath()
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:RLovelett/eigen.git', 'eigen')
    def update(self):
        #did not find a 3.2.1 release on the github fork... using a sha instead
        testedSHA = '36cd8ffd9dfcdded4717efb96daad9f6353f6351'
        self._getBuilder().gitCheckout(testedSHA)
        pass
    def configure(self):
        pass
    def reset(self):
        pass
    def build(self):
        pass
    def makeClean(self):
        pass
    def getBuildType(self):
        pass
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

class CustusX(CppComponent):
    def name(self):
        return "CustusX"
    def help(self):
        return 'CustusX from SINTEF'
    def path(self):
        return self.controlData.getWorkingPath() + "/CustusX"    
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:SINTEFMedisinskTeknologi/CustusX3.git', 'CustusX')
    def update(self):
        self._getBuilder().gitUpdate('master', tag=self.controlData.getGitTag(), submodules=True)    
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        #add('EIGEN_DIR:PATH', '%s/CMake' % self._createSibling(Eigen).configPath())
        add('EIGEN_INCLUDE_DIR:PATH', '%s' % self._createSibling(Eigen).sourcePath())
        add('ITK_DIR:PATH', self._createSibling(ITK).configPath())
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('IGSTK_DIR:PATH', self._createSibling(IGSTK).configPath())
        add('OpenIGTLink_DIR:PATH', self._createSibling(OpenIGTLink).configPath())
        add('OpenCV_DIR:PATH', self._createSibling(OpenCV).configPath())
        add('CTK_DIR:PATH', self._createSibling(CTK).configPath())
        add('Tube-Segmentation-Framework_DIR:PATH', self._createSibling(TubeSegmentationFramework).configPath())
        add('Level-Set-Segmentation_DIR:PATH', self._createSibling(LevelSetSegmentation).configPath())
        add('OpenCLUtilityLibrary_DIR:PATH', self._createSibling(OpenCLUtilityLibrary).configPath())
        add('BUILD_DOCUMENTATION:BOOL', self.controlData.mDoxygen)            
        add('SSC_BUILD_EXAMPLES:BOOL', self.controlData.mBuildSSCExamples);
        add('BUILD_TESTING:BOOL', self.controlData.mBuildTesting);
        add('SSC_USE_GCOV:BOOL', self.controlData.mCoverage);
        
        libs = self.assembly.libraries
        for lib in libs:
            lib.addConfigurationToDownstreamLib(builder)
            if lib.pluginPath() and os.path.exists(lib.pluginPath()):
                add('CX_EXTERNAL_PLUGIN_%s'%lib.name(), lib.pluginPath())
        
        if self.controlData.force_connect_sublibraries:
            self.forceConnectSublibraries(add)
        cmakeOptions = ''
        if self.controlData.mGraphviz:
            cmakeOptions = '--graphviz=graph.dot'
        builder.configureCMake(cmakeOptions)
    def forceConnectSublibraries(self, add):
        print 'CustusX force connect sublibraries.'
        add('BUILD_OPEN_IGTLINK_SERVER:BOOL', True);
#        add('CX_USE_LEVEL_SET:BOOL', platform.system() == 'Linux')
#        add('CX_USE_TSF:BOOL', platform.system() != 'Windows');
        add('CX_PLUGIN_org.custusx.filter.levelset:BOOL', platform.system() == 'Linux');
        add('CX_PLUGIN_org.custusx.filter.tubesegmentation:BOOL', platform.system() != 'Windows');
        add('CX_PLUGIN_org.custusx.ussimulator:BOOL', True);
        add('CX_USE_ISB_GE:BOOL', platform.system() != 'Windows');
        add('CX_BUILD_MEHDI_VTKMULTIVOLUME:BOOL', False);
        
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
        self._getBuilder().gitClone(self.gitRepository(), self.sourceFolder())
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.gitRepository(), 'master')
        self._getBuilder().gitUpdate('master', tag=self.controlData.getGitTag())    
    def configure(self):
        pass
    def build(self):
        pass
    def gitRepository(self):
        return 'git@github.com:SINTEFMedisinskTeknologi/CustusXData.git'
        #return 'medtek.sintef.no//Volumes/medtek_HD/git/Data.git'
    def makeClean(self):
        pass
    def isPubliclyAvailable(self):
        return False
# ---------------------------------------------------------


