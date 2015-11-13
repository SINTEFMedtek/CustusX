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

from cx.utils.cxShell import *    
from cx.utils.cxPrintFormatter import PrintFormatter
import cx.utils.cxCppBuilder

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
        builder = cx.utils.cxCppBuilder.CppBuilder()
        builder.setPaths(base=self.path(), build=self.buildPath(), source=self.sourcePath())
        builder.setBuildType(self.getBuildType())
        builder.setControlData(self.controlData)
        return builder
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
        self._getBuilder().gitCheckout('v4.6.1')
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
        base = self.controlData.gitrepo_open_site_base
        self._getBuilder().gitClone('%s/VTK' % base)
    def update(self):
        # this fix should rebase repo from the original Kitware/VTK to our own fork on GitHub.
        repo = '%s/VTK' % self.controlData.gitrepo_open_site_base
        branch = 'VTK-CX-modifications'
        #tag = 'VTK-6-1-0.cx_patch_2'
        tag = 'VTK-6-2-0.cx_patch_1'
        self._getBuilder().gitSetRemoteURL(repo, branch=branch)
        self._getBuilder().gitCheckout(tag)
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('VTK_USE_PARALLEL:BOOL', 'ON')
        add('VTK_REQUIRED_OBJCXX_FLAGS:STRING', "")
        add('VTK_USE_RPATH:BOOL', 'ON')
        
        use_qt5 = True
        if use_qt5:
#            qt5_root = '/Users/christiana/Qt/5.3/clang_64'
            add('VTK_QT_VERSION:STRING', "5")
#            add('QT_QMAKE_EXECUTABLE:PATH', "%s/bin/qmake"%qt5_root)
            add('VTK_Group_Qt:BOOL', "ON")
#            add('CMAKE_PREFIX_PATH:PATH', "%s/lib/cmake"%qt5_root)
            add('CMAKE_PREFIX_PATH:PATH', "/opt/local/libexec/qt5-mac")
        else:
            add('DESIRED_QT_VERSION:STRING', 4)
            add('Module_vtkGUISupportQt:BOOL', 'ON')
            add('VTK_USE_PARALLEL:BOOL', 'ON')
            add('VTK_USE_RPATH:BOOL', 'ON')        
        
        add('BUILD_TESTING:BOOL', self.controlData.mBuildExAndTest)
        add('BUILD_EXAMPLES:BOOL', self.controlData.mBuildExAndTest)
        add('Module_vtkGUISupportQt:BOOL', 'ON')
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
    def _getRepo(self):
        base = self.controlData.gitrepo_open_site_base
        return '%s/CTK.git' % base
    def _rawCheckout(self):
        self._getBuilder().gitClone(self._getRepo())
    def update(self):
        repo = self._getRepo()
        branch = 'CTK-CX-modifications'
        self._getBuilder().gitSetRemoteURL(repo, branch=branch)
        #self._getBuilder().gitCheckout('1056228ab4aeefa9bf6db4fc32a8826db283475a') 
        self._getBuilder().gitCheckout('f9478bdb4d5ecc6357b3a579a4bf8a927debd5e3') # new 2015-10-19
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('CTK_QT_VERSION:STRING', 5)
        add('CTK_ENABLE_DICOM:BOOL', 'ON')
        add('CTK_LIB_DICOM/Widgets:BOOL', 'ON')
        add('CTK_ENABLE_PluginFramework:BOOL', 'ON')
        #add('CTK_BUILD_SHARED_LIBS:BOOL', self.controlData.getBuildShared())
        add('CTK_BUILD_SHARED_LIBS:BOOL', 'ON')
        add('CMAKE_PREFIX_PATH:PATH', "/opt/local/libexec/qt5-mac")
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
        self._getBuilder().gitCheckout('2.4.11')
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
        add('WITH_FFMPEG:BOOL', False)
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
        self._getBuilder().gitCheckout('5a501817c2da52e81db4db3eca6dd5111f94fed9')
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('BUILD_TESTING:BOOL', False)
        add('BUILD_EXAMPLES:BOOL', False)
        builder.configureCMake()
    def addConfigurationToDownstreamLib(self, builder):
        add = builder.addCMakeOption
        add('BUILD_OPEN_IGTLINK_SERVER:BOOL', 'ON');
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
        base = self.controlData.gitrepo_open_site_base
        repo = '%s/IGSTK' % base
        branch = 'IGSTK-CX-modifications'
        self._getBuilder().gitSetRemoteURL(repo, branch=branch)
        self._getBuilder().gitCheckout('d29479aa7a5e2679a879105b7c11c8a9c8eaa7cf')
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
        return 'custusx.org'
    def path(self):
        loc = self.controlData.getCustusXRepositoryLocation()
        return '%s/%s' % (loc[0], loc[1])    
        #return '%s/%s' % (self.controlData.getWorkingPath(), self.sourceFolder())    
    def sourceFolder(self):
        return self.controlData.getRepoFolderName()
    def _rawCheckout(self):
        base = self.controlData.gitrepo_open_site_base    
        repo = '%s/CustusX.git' % base
        self._getBuilder().gitCloneIntoExistingDirectory(repo, self.controlData.main_branch)
    def update(self):
        self._getBuilder().gitCheckoutDefaultBranch(submodules=True)    
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('EIGEN_INCLUDE_DIR:PATH', '%s' % self._createSibling(Eigen).sourcePath())
        add('ITK_DIR:PATH', self._createSibling(ITK).configPath())
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('IGSTK_DIR:PATH', self._createSibling(IGSTK).configPath())
        add('OpenIGTLink_DIR:PATH', self._createSibling(OpenIGTLink).configPath())
        add('OpenCV_DIR:PATH', self._createSibling(OpenCV).configPath())
        add('CTK_SOURCE_DIR:PATH', self._createSibling(CTK).sourcePath())
        add('CTK_DIR:PATH', self._createSibling(CTK).configPath())
        add('Tube-Segmentation-Framework_DIR:PATH', self._createSibling(TubeSegmentationFramework).configPath())
        add('Level-Set-Segmentation_DIR:PATH', self._createSibling(LevelSetSegmentation).configPath())
        add('OpenCLUtilityLibrary_DIR:PATH', self._createSibling(OpenCLUtilityLibrary).configPath())
        add('BUILD_DOCUMENTATION:BOOL', self.controlData.build_developer_doc)            
        add('CX_BUILD_USER_DOCUMENTATION:BOOL', self.controlData.build_user_doc)            
        add('BUILD_TESTING:BOOL', self.controlData.mBuildTesting);
        add('SSC_USE_GCOV:BOOL', self.controlData.mCoverage);
        add('CX_SYSTEM_BASE_NAME:STRING', self.controlData.system_base_name)
        add('CX_SYSTEM_DEFAULT_APPLICATION:STRING', self.controlData.system_base_name)
        add('CMAKE_PREFIX_PATH:PATH', "/opt/local/libexec/qt5-mac")
        
        
        libs = self.assembly.libraries
        for lib in libs:
            lib.addConfigurationToDownstreamLib(builder)
            if lib.pluginPath() and os.path.exists(lib.pluginPath()):
                add('CX_EXTERNAL_PLUGIN_%s'%lib.name(), lib.pluginPath())
        
        cmakeOptions = ''
        if self.controlData.mGraphviz:
            cmakeOptions = '--graphviz=graph.dot'
        builder.configureCMake(cmakeOptions)
        
# ---------------------------------------------------------


class TubeSegmentationFramework(CppComponent):
    def name(self):
        return "Tube-Segmentation-Framework"
    def help(self):
        return 'Tube-Segmentation-Framework'
    def path(self):
        return self.controlData.getWorkingPath() + "/Tube-Segmentation-Framework"
    def _rawCheckout(self):
        base = self.controlData.gitrepo_open_site_base
        self._getBuilder().gitClone('%s/Tube-Segmentation-Framework.git' % base)
    def update(self):
        self._getBuilder().gitCheckout('9faceef98c6ee943a1301b0d57f9db0deb7e59e9')
        self._getBuilder()._gitSubmoduleUpdate()
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('USE_C++11:BOOL', False)
        add('SIPL_USE_GTK:BOOL', False)
        add('sipl_use_gtk:BOOL', False) #variables in cmake are case sensitive, SIPL uses this options
        add('TSF_USE_EXTRNAL_OUL:BOOL', 'ON')
        add('TSF_EXTERNAL_OUL_PATH:PATH', self._createSibling(OpenCLUtilityLibrary).findPackagePath())
        if(platform.system() == 'Windows'):
            add('BUILD_SHARED_LIBS:BOOL', 'OFF') #On windows we build TSF as static, because TSF and SIPL does not export symbols
        builder.configureCMake()
    def addConfigurationToDownstreamLib(self, builder):
        add = builder.addCMakeOption
        add('CX_PLUGIN_org.custusx.filter.tubesegmentation:BOOL', platform.system() != 'Windows');

 # ---------------------------------------------------------

class LevelSetSegmentation(CppComponent):
    def name(self):
        return "Level-Set-Segmentation"
    def help(self):
        return 'Level-Set-Segmentation'
    def path(self):
        return self.controlData.getWorkingPath() + "/Level-Set-Segmentation"
    def _rawCheckout(self):
        base = self.controlData.gitrepo_open_site_base
        self._getBuilder().gitClone('%s/Level-Set-Segmentation' % base)
    def update(self):
        # this fix should rebase repo from the original smistad/LSS to our own fork on GitHub.
        repo = '%s/Level-Set-Segmentation' % self.controlData.gitrepo_open_site_base
        self._getBuilder().gitSetRemoteURL(repo, branch="master")
        self._getBuilder().gitCheckout('e49217188925845be9b336adcb2b9e81c26ea784')
        self._getBuilder()._gitSubmoduleUpdate()
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('sipl_use_gtk:BOOL', 'OFF')
        add('LS_USE_EXTRNAL_OUL:BOOL', 'ON')
        add('LS_EXTERNAL_OUL_PATH:PATH', self._createSibling(OpenCLUtilityLibrary).findPackagePath())
        builder.configureCMake()
    def addConfigurationToDownstreamLib(self, builder):
        add = builder.addCMakeOption
        add('CX_PLUGIN_org.custusx.filter.levelset:BOOL', platform.system() == 'Linux');
        
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
        #self._getBuilder().gitCheckoutBranch('custusx', submodules=False)
        self._getBuilder().gitCheckout('43614718f7667dd5013af9300fcc63ae30bf244c')
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
        self._getBuilder().gitSetRemoteURL(self.gitRepository(), self.controlData.main_branch)
        self._getBuilder().gitCheckoutDefaultBranch(submodules=True)    
    def configure(self):
        pass
    def build(self):
        pass
    def gitRepository(self):
        base = self.controlData.gitrepo_open_site_base
        return '%s/CustusXData.git' % base
    def makeClean(self):
        pass
# ---------------------------------------------------------

class QHttpServer(CppComponent):
    def name(self):
        return "QHttpServer"
    def help(self):
        return 'https://github.com/nikhilm/qhttpserver'
    def path(self):
        return self.controlData.getExternalPath() + "/QHttpServer"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def _rawCheckout(self):
        self._getBuilder().gitClone('git@github.com:SINTEFMedtek/qhttpserver.git %s' % self.sourceFolder())
    def update(self):
        self._getBuilder().gitCheckout('5eb5a89f23a3948162bf8fcaa94363777d723482') # latest tested SHA
#    def configure(self):
#        builder = self._getBuilder()
#        changeDir(self.buildPath())
#        shell.run('qmake %s' % self.sourcePath())
    def configure(self):
        builder = self._getBuilder()
        builder.configureCMake()    
#    def build(self):
#        changeDir(self.buildPath())
#        shell.run('make')
#    def makeClean(self):
#        changeDir(self.buildPath())
#        shell.run('make clean')
    def addConfigurationToDownstreamLib(self, builder):
        add = builder.addCMakeOption
        add('qhttpserver_DIR:PATH', self.buildPath())
        #add('QHTTPSERVERCPP_LIBRARY_DIR:PATH', self.buildPath()+'/lib')
        #add('QHTTPSERVERCPP_INCLUDE_DIR:PATH', self.sourcePath()+'/src')
        #add('QHttpServer_ROOT_DIR:PATH', self.path())
        #add('CX_PLUGIN_org.custusx.filter.levelset:BOOL', platform.system() == 'Linux');

