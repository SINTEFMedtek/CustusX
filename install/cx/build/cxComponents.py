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
    def sourceFolder(self):
        return self.name()
    def path(self):
        return self.controlData.getWorkingPath() + "/" + self.name()
#    def path(self):
#        'return path where component will be installed'
#        raise "Not Implemented"
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
#    def _rawCheckout(self):
#        'checkout the component source from external source to this computer (svn co or similar)'
#        raise "Not Implemented"
    def _rawCheckout(self):
        'checkout the component source from external source to this computer (svn co or similar)'
        repo = self.repository()
        if repo=="":
            raise "Not Implemented"            
        #self.sourceFolder()
        self._getBuilder().gitClone(self.repository(), self.sourceFolder())
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
    def useInIntegrationTesting(self):
        'use during integration test'
        return False
    def repository(self):
        'url of repository'
        return ""
    def useExternalRepositories(self):
        return self.controlData.gitrepo_main_site_base == self.controlData.gitrepo_open_site_base

        
# ---------------------------------------------------------

class CppComponent(Component):
    '''
    Implementation of Cpp-style components, i.e all c++ libraries
    that contains a source and build folder, and are built with make.
    Inherit from this to get some methods for free.
    '''
    def __init__(self):
        pass
    def buildFolder(self):
        return self.controlData.getBuildFolder(self.name())
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
#    def path(self):
#        return self.controlData.getExternalPath() + "/ITK"
    def sourceFolder(self):
        return 'ITK'
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('v4.12.0')
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('BUILD_TESTING:BOOL', self.controlData.mBuildExAndTest)
        add('BUILD_EXAMPLES:BOOL', self.controlData.mBuildExAndTest)
        builder.configureCMake()
    def repository(self):
        if self.useExternalRepositories():
            return 'git://itk.org/ITK.git'
        else:
            return '%s/ITK.git' % self.controlData.gitrepo_main_site_base
# ---------------------------------------------------------

class VTK(CppComponent):
    def name(self):
        return "VTK"
    def help(self):
        return 'vtk.org'
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def repository(self):
        return '%s/VTK' % self.controlData.gitrepo_open_site_base
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('1c14943c3975fe826da1e7be1624c16c893d1c68')
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('VTK_USE_PARALLEL:BOOL', 'ON')
        add('VTK_REQUIRED_OBJCXX_FLAGS:STRING', "")
        add('VTK_USE_RPATH:BOOL', 'ON')
        
        use_qt5 = True
        if use_qt5:
            add('VTK_QT_VERSION:STRING', "5")
            add('VTK_Group_Qt:BOOL', "ON")
            add('CMAKE_PREFIX_PATH:PATH', "/opt/local/libexec/qt5-mac")
        else:
            add('DESIRED_QT_VERSION:STRING', 4)
            add('Module_vtkGUISupportQt:BOOL', 'ON')
            add('VTK_USE_PARALLEL:BOOL', 'ON')
            add('VTK_USE_RPATH:BOOL', 'ON')        
        
        add('BUILD_TESTING:BOOL', self.controlData.mBuildExAndTest)
        add('BUILD_EXAMPLES:BOOL', self.controlData.mBuildExAndTest)
        add('Module_vtkGUISupportQt:BOOL', 'ON')
        add('VTK_RENDERING_BACKEND:STRING', "OpenGL2")
        builder.configureCMake()
# ---------------------------------------------------------

class CTK(CppComponent):
    def name(self):
        return "CTK"
    def help(self):
        return 'commontk.org'
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def repository(self):
        base = self.controlData.gitrepo_open_site_base
        return '%s/CTK.git' % base
    def update(self):
        self._getBuilder().gitCheckout('4af01a590b293117af49d105a44224e32e5272c9')
        self._getBuilder().gitSetRemoteURL(self.repository())
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('CTK_QT_VERSION:STRING', 5)
        add('CTK_ENABLE_DICOM:BOOL', 'ON')
        add('CTK_LIB_DICOM/Widgets:BOOL', 'ON')
        add('CTK_ENABLE_PluginFramework:BOOL', 'ON')
        add('CTK_BUILD_SHARED_LIBS:BOOL', 'ON')
        add('CMAKE_PREFIX_PATH:PATH', "/opt/local/libexec/qt5-mac")
        add('CTK_LIB_Visualization/VTK/Core:BOOL', 'ON')
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('BUILD_TESTING:BOOL', 'OFF')
        builder.configureCMake()
        PrintFormatter.printInfo('Build CTK during configure step, in order to create CTKConfig.cmake')
        self.build()
# ---------------------------------------------------------

class OpenCV(CppComponent):
    def name(self):
        return "OpenCV"
    def help(self):
        return 'http://opencv.willowgarage.com'
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def repository(self):
        if self.useExternalRepositories():
           return 'https://github.com/Itseez/opencv.git'
        else:
            return '%s/OpenCV.git' % self.controlData.gitrepo_main_site_base
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('3.3.0')
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
        add('WITH_GSTREAMER:BOOL', False)
        builder.configureCMake()
# ---------------------------------------------------------

class Eigen(CppComponent):
    def name(self):
        return "eigen"
    def help(self):
        return 'http://eigen.tuxfamily.org/'
    def configPath(self):
        return self.sourcePath()
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def repository(self):
        return 'git@github.com:eigenteam/eigen-git-mirror.git'
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        #See CX-208 about updating Eigen versions
        tag = '3.3.5'
        self._getBuilder().gitCheckout(tag)
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
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def repository(self):
        return 'git://github.com/openigtlink/OpenIGTLink.git'
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
#        self._getBuilder().gitCheckout('805472b43aebf96fec0b62b2898a24446fe19c08') # Previous version used by CustusX
        self._getBuilder().gitCheckout('4c39d0fcd26db74022b5b891a9b274c51362cb28') # Latest version
#        self._getBuilder().gitCheckoutBranch('master')#TODO: Switch to a sha before merging the branch back to develop
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

class OpenIGTLinkIO(CppComponent):
    def name(self):
        return "OpenIGTLinkIO"
    def help(self):
        return 'https://github.com/IGSIO/OpenIGTLinkIO'
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def repository(self):
        return 'git@github.com:IGSIO/OpenIGTLinkIO.git'
#        return 'git@github.com:SINTEFMedtek/OpenIGTLinkIO.git'
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('f862d6bfa270332955e8050df8ce057febf48392')
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('CTK_DIR:PATH', self._createSibling(CTK).configPath())
        add('OpenIGTLink_DIR:PATH', self._createSibling(OpenIGTLink).configPath())
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
#    def path(self):
#        return self.controlData.getExternalPath() + "/IGSTK"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def repository(self):
        base = self.controlData.gitrepo_open_site_base
        repo = '%s/IGSTK.git' % base
        return repo
    def update(self):
        branch = 'IGSTK-CX-modifications'
        self._getBuilder().gitSetRemoteURL(self.repository(), branch=branch)
        self._getBuilder().gitCheckout('bda6b6fa88054b474aa113af5477813610e4ac3b')
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
    def repository(self):
        return '%s/CustusX.git' % self.controlData.gitrepo_main_site_base
    def _rawCheckout(self):
        pass # should never happen. This file is in the repo.
        #self._getBuilder().gitCloneIntoExistingDirectory(self.repository(), self.controlData.main_branch)
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        # warning: if this call checks out a different tag/branch than the current, 
        # the script and code will be inconsistent. The user should have set the correct
        # tag/branch either manually or by using a wrapper script (e.g cxCustusXFinder).
        self._getBuilder().gitCheckoutDefaultBranch()    
    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        append = builder.appendCMakeOption
        add('EIGEN_INCLUDE_DIR:PATH', '%s' % self._createSibling(Eigen).sourcePath())
        add('ITK_DIR:PATH', self._createSibling(ITK).configPath())
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        add('IGSTK_DIR:PATH', self._createSibling(IGSTK).configPath())
        add('OpenIGTLink_DIR:PATH', self._createSibling(OpenIGTLink).configPath())
        add('OpenIGTLinkIO_DIR:PATH', self._createSibling(OpenIGTLinkIO).configPath())
        add('OpenCV_DIR:PATH', self._createSibling(OpenCV).configPath())
        add('CTK_SOURCE_DIR:PATH', self._createSibling(CTK).sourcePath())
        add('CTK_DIR:PATH', self._createSibling(CTK).configPath())
        add('OpenCLUtilityLibrary_DIR:PATH', self._createSibling(OpenCLUtilityLibrary).configPath())
        add('FAST_DIR:PATH', self._createSibling(FAST).configPath())
        add('BUILD_DOCUMENTATION:BOOL', self.controlData.build_developer_doc)            
        add('CX_BUILD_USER_DOCUMENTATION:BOOL', self.controlData.build_user_doc)            
        add('BUILD_TESTING:BOOL', self.controlData.mBuildTesting);
        add('SSC_USE_GCOV:BOOL', self.controlData.mCoverage);
        add('CX_SYSTEM_BASE_NAME:STRING', self.controlData.system_base_name)
        add('CX_SYSTEM_DEFAULT_APPLICATION:STRING', self.controlData.system_base_name)
        add('CMAKE_PREFIX_PATH:PATH', "/opt/local/libexec/qt5-mac")
        # See CX-208 about this Eigen flag and about updating Eigen.
        # The second one should be used when upgrading
        # to version > 3.2, as the old one is depracated in version 3.3.
        append('CX_CMAKE_CXX_FLAGS:STRING', '-DEIGEN_DONT_ALIGN')
        #append('CMAKE_CXX_FLAGS:STRING', '-DEIGEN_MAX_ALIGN_BYTES=0')
        
        
        libs = self.assembly.libraries
        for lib in libs:
            lib.addConfigurationToDownstreamLib(builder)
            if lib.pluginPath() and os.path.exists(lib.pluginPath()):
                add('CX_EXTERNAL_PLUGIN_%s'%lib.name(), lib.pluginPath())
        
        cmakeOptions = ''
        if self.controlData.mGraphviz:
            cmakeOptions = '--graphviz=graph.dot'
        builder.configureCMake(cmakeOptions)
    def useInIntegrationTesting(self):
        'use during integration test'
        return True
        
# ---------------------------------------------------------

class OpenCLUtilityLibrary(CppComponent):
    def name(self):
        return "OpenCLUtilityLibrary"
    def help(self):
        return 'OpenCLUtilityLibrary. Functions for interacting with OpenCL.'
#    def path(self):
#        return self.controlData.getWorkingPath() + "/OpenCLUtilityLibrary"
    def repository(self):
        return 'git@github.com:smistad/OpenCLUtilityLibrary'
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('44b7a002195fb2b6e8ea99ea4edf3102ef556cc3')
    def configure(self):
        builder = self._getBuilder()
        builder.configureCMake()
    def findPackagePath(self):
        return self.buildPath()
        
# ---------------------------------------------------------

class FAST(CppComponent):
    def name(self):
        return "FAST"
    def help(self):
        return 'FAST.'
#    def path(self):
#        return self.controlData.getWorkingPath() + "/FAST"
    def sourcePath(self):
        return self.controlData.getWorkingPath() + "/FAST/FAST/"
    def repository(self):
        return 'git@github.com:smistad/FAST'
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('173bb92c0c2f1c57aff9c26e06db290d80fbcf83')
#        branch = 'set_kernel_root_dir'
#        self._getBuilder()._changeDirToSource()
#        runShell('git checkout %s' % branch, ignoreFailure=False)
#        runShell('git pull origin %s' % branch, ignoreFailure=False)

    def configure(self):
        builder = self._getBuilder()
        add = builder.addCMakeOption
        append = builder.appendCMakeOption
        add('FAST_MODULE_OpenIGTLink:BOOL', False)
        add('FAST_MODULE_Visualization:BOOL', False)
        add('FAST_MODULE_Python:BOOL', False)
        add('FAST_MODULE_NeuralNetwork:BOOL', False)
        add('FAST_MODULE_VTK:BOOL', True)
        add('FAST_DOWNLOAD_TEST_DATA:BOOL', False)
        add('FAST_BUILD_EXAMPLES:BOOL', False)
        add('FAST_BUILD_TOOLS:BOOL', False)
        add('FAST_BUILD_TESTS:BOOL', False)
        add('VTK_DIR:PATH', self._createSibling(VTK).configPath())
        if(platform.system() == 'Windows'):
            add('BUILD_SHARED_LIBS:BOOL', 'OFF')
        append('CMAKE_CXX_FLAGS:STRING', '-DEIGEN_MAX_ALIGN_BYTES=0')
        builder.configureCMake()
    def findPackagePath(self):
        return self.buildPath()
    def addConfigurationToDownstreamLib(self, builder):
        add = builder.addCMakeOption
        add('CX_PLUGIN_org.custusx.filter.airways:BOOL', True);
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
    def repository(self):
        return '%s/CustusXData.git' % self.controlData.gitrepo_main_site_base
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('35560c25a6ef29a3b0abdedfb870e38f52d19ee9')
    def configure(self):
        pass
    def build(self):
        pass
    def makeClean(self):
        pass
    def useInIntegrationTesting(self):
        'use during integration test'
        return True
# ---------------------------------------------------------

class QHttpServer(CppComponent):
    def name(self):
        return "QHttpServer"
    def help(self):
        return 'https://github.com/nikhilm/qhttpserver'
#    def path(self):
#        return self.controlData.getExternalPath() + "/QHttpServer"
    def getBuildType(self):
        return self.controlData.getBuildExternalsType()
    def repository(self):
        return 'git@github.com:SINTEFMedtek/qhttpserver.git'
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('5b7d7e15cfda2bb2097b6c0ceab99eeb50b4f639') # latest tested SHA
    def configure(self):
        builder = self._getBuilder()
        builder.configureCMake()    
    def addConfigurationToDownstreamLib(self, builder):
        add = builder.addCMakeOption
        add('qhttpserver_DIR:PATH', self.buildPath())

# ---------------------------------------------------------

class org_custusx_angleCorrection(CppComponent):

    def name(self):
        return "org.custusx.anglecorrection"
    def help(self):
        return 'Plugin for Angle correction of velocities'
    def path(self):
        custusx = self._createSibling(cx.build.cxComponents.CustusX)
        return '%s/%s/source/plugins' % (custusx.path(), custusx.sourceFolder())
    def sourceFolder(self):
        return 'org.custusx.anglecorrection'
#    def _rawCheckout(self):
#        self._getBuilder().gitClone(self.gitRepository(), self.sourceFolder())
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
#        self._getBuilder().gitCheckout('0.1')
        self._getBuilder().gitCheckout('01acc6547c95e506b88c20011789784a129a16bb')
    def configure(self):
        pass
    def build(self):
        pass
    def repository(self):
        return 'git@github.com:SINTEFMedtek/AngleCorr.git'
    def makeClean(self):
        pass
    def pluginPath(self):
        return '%s' % self.sourcePath()

# ---------------------------------------------------------
# These are private repositories, not part of the CustusX open source build
# ---------------------------------------------------------

class org_custusx_mariana(CppComponent):

    def name(self):
        return "org.custusx.mariana"
    def help(self):
        return 'Plugin for the Mariana project'
    def path(self):
        custusx = self._createSibling(cx.build.cxComponents.CustusX)
        return '%s/%s/source/plugins' % (custusx.path(), custusx.sourceFolder())
    def sourceFolder(self):
        return 'org.custusx.mariana'
    def update(self):
      self._getBuilder().gitSetRemoteURL(self.repository())
#      self._getBuilder().gitCheckout('')
#      self._getBuilder().gitCheckoutDefaultBranch()#Not using sha here because we always want to use the develop branch in the Mariana repo
      self._getBuilder().gitCheckout('55d76dddf8750e4b2198265d97d432b0a02dcece')
    def configure(self):
        pass
    def build(self):
        pass
    def repository(self):
        return 'git@github.com:SINTEFMedtek/Mariana.git'
    def makeClean(self):
        pass
    def pluginPath(self):
        return '%s' % self.sourcePath()
#    def addConfigurationToDownstreamLib(self, builder):
#        add = builder.addCMakeOption
#        add('CX_PLUGIN_org.custusx.mariana:BOOL', 'ON');

# ---------------------------------------------------------

class org_custusx_bronchoscopynavigation(CppComponent):

    def name(self):
        return "org.custusx.bronchoscopynavigation"
    def help(self):
        return 'Plugin bronchoscopynavigation'
    def path(self):
        custusx = self._createSibling(cx.build.cxComponents.CustusX)
        return '%s/%s/source/plugins' % (custusx.path(), custusx.sourceFolder())
    def sourceFolder(self):
        return 'org.custusx.bronchoscopynavigation'
#    def _rawCheckout(self):
#        self._getBuilder().gitClone(self.gitRepository(), self.sourceFolder())
    def update(self):
        self._getBuilder().gitSetRemoteURL(self.repository())
        self._getBuilder().gitCheckout('54571ecc3bdd5c993bf615d04229bfc6d323b192')
    def configure(self):
        pass
    def build(self):
        pass
    def repository(self):
        base = self.controlData.gitrepo_open_site_base
        return '%s/org.custusx.bronchoscopynavigation.git' % base
    def makeClean(self):
        pass
    def pluginPath(self):
        return '%s' % self.sourcePath()
