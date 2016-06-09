/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxPluginFramework.h"

#include <QApplication>
#include <QStringList>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

#include "ctkPluginFrameworkFactory.h"
#include "ctkPluginFramework.h"
#include "ctkPluginContext.h"
#include "ctkPluginException.h"

#include <ctkConfig.h>

#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxPluginFrameworkUtilities.h"

#include "cxFileHelpers.h"
#include "cxLogger.h"
#include <iostream>
#include "cxTypeConversions.h"
#include "cxProfile.h"

namespace cx
{

PluginFrameworkManager::PluginFrameworkManager()
{
	mSettingsBase = "pluginFramework";
	mSettingsSearchPaths = mSettingsBase + "/searchPaths";

	ctkProperties fwProps;
	QString storagePath = ProfileManager::getInstance()->getSettingsPath() + "/pluginFramework";

	fwProps[ctkPluginConstants::FRAMEWORK_STORAGE] = storagePath;

	// remove settings as stored by CTK, because full paths are stored here, causing
	// problems when running both debug and release on the same machine (and similar).
	fwProps[ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN] = ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT;

	mFrameworkFactory.reset(new ctkPluginFrameworkFactory(fwProps));
	mPluginLibFilter << "*.dll" << "*.so" << "*.dylib";
}

PluginFrameworkManager::~PluginFrameworkManager()
{
	if(mFramework->getState() == ctkPlugin::ACTIVE)//LogicManager calls stop() before the destructor is called
	{
		CX_LOG_CHANNEL_WARNING("plugin") << "This should not happen: PluginFrameworkManager destructor stopping plugin framework";
		this->stop();
	}
}

QString PluginFrameworkManager::convertToRelativePath(QString path) const
{
	QDir base = qApp->applicationDirPath();
	return base.relativeFilePath(path);
}

QString PluginFrameworkManager::convertToAbsolutePath(QString path) const
{
	if (QDir(path).isAbsolute())
		return QDir(path).absolutePath();

	QDir base = qApp->applicationDirPath();
	return QDir(base.path() + "/" + path).absolutePath();
}



std::vector<PluginFrameworkManager::PluginLoadInfo> PluginFrameworkManager::getPluginLoadInfo(QStringList symbolicNames)
{
	std::vector<PluginLoadInfo> retval;

	for (unsigned i=0; i<symbolicNames.size(); ++i)
	{
		PluginLoadInfo info;
		info.symbolicName = symbolicNames[i];
		info.storedState = settings()->value(mSettingsBase+"/"+info.symbolicName).toString();

		if (info.storedState.isEmpty())
		{
			info.isNew = true;
			info.storedState = getStringForctkPluginState(ctkPlugin::ACTIVE);
		}
		else
		{
			info.isNew = false;
		}

		info.targetState = getctkPluginStateForString(info.storedState);

		retval.push_back(info);
	}

	return retval;
}

void PluginFrameworkManager::loadState()
{
    CX_LOG_DEBUG() << "mSettingsSearchPaths: " << mSettingsSearchPaths;
	QStringList paths = settings()->value(mSettingsSearchPaths, QStringList()).toStringList();
	this->setSearchPaths(paths);
    CX_LOG_DEBUG() << "mSettingsSearchPaths: " << mSettingsSearchPaths;
    CX_LOG_DEBUG() << "paths: " << paths.join(", ");

	QStringList names = this->getPluginSymbolicNames();
	std::vector<PluginLoadInfo> info = this->getPluginLoadInfo(names);

	// install all plugins, must do this first in order to let FW handle dependencies.
	CX_LOG_CHANNEL_INFO("plugin") << "Installing all plugins...";
	for (unsigned i=0; i< info.size(); ++i)
	{
		if (info[i].targetState != ctkPlugin::UNINSTALLED)
			this->install(info[i].symbolicName);
	}

    CX_LOG_DEBUG() << "start all plugins";
	// start all plugins
	for (unsigned i=0; i< info.size(); ++i)
	{
		if (info[i].targetState == ctkPlugin::ACTIVE)
		{
			if (info[i].isNew)
				CX_LOG_CHANNEL_INFO("plugin") << QString("Autostarting plugin %1").arg(info[i].symbolicName);
			else
				CX_LOG_CHANNEL_INFO("plugin") << QString("Starting plugin %1").arg(info[i].symbolicName);

			this->start(info[i].symbolicName, ctkPlugin::START_TRANSIENT);
		}
		else
		{
			CX_LOG_CHANNEL_INFO("plugin") << QString("Set plugin to state [%2]: %1")
											 .arg(info[i].symbolicName)
											 .arg(info[i].storedState);
		}
	}

}

void PluginFrameworkManager::saveState()
{
	QStringList relativePaths;
	for (int i=0; i<mPluginSearchPaths.size(); ++i)
		relativePaths << this->convertToRelativePath(mPluginSearchPaths[i]);
	settings()->setValue(mSettingsSearchPaths, relativePaths);

	QStringList names = this->getPluginSymbolicNames();
	for (unsigned i=0; i<names.size(); ++i)
	{
		QString name = names[i];
		ctkPlugin::State state = this->getStateFromSymbolicName(name);
		settings()->setValue(mSettingsBase+"/"+name, getStringForctkPluginState(state));
	}
}

ctkPlugin::State PluginFrameworkManager::getStateFromSymbolicName(QString name)
{
	ctkPlugin::State state = ctkPlugin::UNINSTALLED;
	QSharedPointer<ctkPlugin> plugin = this->getInstalledPluginFromSymbolicName(name);
	if (plugin)
		state = plugin->getState();
	return state;
}

void PluginFrameworkManager::setSearchPaths(const QStringList& searchPath)
{
	mPluginSearchPaths.clear();

	for (int i=0; i<searchPath.size(); ++i)
		mPluginSearchPaths <<  this->convertToAbsolutePath(searchPath[i]);

	QStringList defPaths = DataLocations::getDefaultPluginsPath();
	for (unsigned i=0; i<defPaths.size(); ++i)
	{
		QString defPath = this->convertToAbsolutePath(defPaths[i]);
		if (!mPluginSearchPaths.count(defPath))
			mPluginSearchPaths << defPath;
	}

	mPluginSearchPaths.removeDuplicates();

	for (int i=0; i<searchPath.size(); ++i)
	{
		QApplication::addLibraryPath(searchPath[i]);
	}
	emit pluginPoolChanged();
}


QStringList PluginFrameworkManager::getSearchPaths() const
{
	return mPluginSearchPaths;
}

ctkPluginContext* PluginFrameworkManager::getPluginContext()
{
	return mFramework->getPluginContext();
}

QSharedPointer<ctkPluginFramework> PluginFrameworkManager::getPluginFramework()
{
	return mFramework;
}

void PluginFrameworkManager::initializeFramework()
{
	if (this->frameworkInitialized())
		return;

	QSharedPointer<ctkPluginFramework> framework = mFrameworkFactory->getFramework();

	try
	{
		framework->init();
	} catch (const ctkPluginException& exc)
	{
		qCritical() << "Failed to initialize the plug-in framework:" << exc;
	}
	mFramework = framework;
}

bool PluginFrameworkManager::frameworkInitialized() const
{
	return mFramework != 0;
}

bool PluginFrameworkManager::frameworkStarted() const
{
	return mFramework && (mFramework->getState() == ctkPlugin::ACTIVE);
}

void PluginFrameworkManager::startFramework()
{
	if (!this->frameworkInitialized())
		this->initializeFramework();

	if (this->frameworkStarted())
		return;

	try
	{
		mFramework->start();
	}
	catch (const ctkPluginException& exc)
	{
		qCritical() << "Failed to start the plug-in framework:" << exc;
	}
}

void PluginFrameworkManager::install(const QString& symbolicName)
{
    CX_LOG_DEBUG() << "PluginFrameworkManager::install: " << symbolicName;
	this->initializeFramework();
	if (!this->frameworkInitialized())
		return;

	QString pluginPath = this->getPluginPath(symbolicName);
	if (pluginPath.isEmpty())
		return;

	try
	{
		ctkPluginContext* pc = this->getPluginContext();
		pc->installPlugin(QUrl::fromLocalFile(pluginPath))->getPluginId();
	}
	catch (const ctkPluginException& exc)
	{
		CX_LOG_CHANNEL_ERROR("plugin") << "Failed to install plugin:" << symbolicName << ", " << exc.what();
	}
	catch (const ctkRuntimeException& exc)
	{
		CX_LOG_CHANNEL_ERROR("plugin")
				<< QString("Failed to install plugin (runtime error): %1, %2")
				   .arg(symbolicName)
				   .arg(exc.what());
	}
}

bool PluginFrameworkManager::start()
{
	this->startFramework();
	return this->frameworkStarted();
}

bool PluginFrameworkManager::stop()
{
    this->saveState();

	// give plugins time to clean up internal resources before different thread deletes them
	// (obsolete because we have disabled the other-thread shutdown)
    emit aboutToStop();

	// Bypass CTK internal 'shutdown in another thread'-mechanism, activated if we
	// call framework::stop(). It causes too much trouble regarding Qt objects created
	// in main thread and deleted in another thread. openCV also has trouble.
	QStringList plugins = getPluginSymbolicNames();
	for (int i=0; i<plugins.size(); ++i)
	{
		this->stop(plugins[i]);
	}

	// stop the framework
	try
	{
        mFramework->stop(); //will start a thread that destructs plugins
		ctkPluginFrameworkEvent fe = mFramework->waitForStop(5000);
//		int timeout = 5000;
//		int interval = 50;
//		ctkPluginFrameworkEvent fe;
//		for(int i=0; i<timeout/interval; ++i)
//		{
//			fe = mFramework->waitForStop(interval);
//			qApp->processEvents();
//		}
		if (fe.getType() == ctkPluginFrameworkEvent::FRAMEWORK_WAIT_TIMEDOUT)
		{
			CX_LOG_CHANNEL_WARNING("plugin") << "Stopping the plugin framework timed out";
			return false;
		}
	}
	catch (const ctkRuntimeException& e)
	{
		CX_LOG_CHANNEL_WARNING("plugin") << QString("Stopping the plugin framework failed: %1").arg(e.what());
		return false;
	}
    return !this->frameworkStarted();
}

void PluginFrameworkManager::uninstall(const QString& symbolicName)
{
	QString pluginPath = getPluginPath(symbolicName);
	if (pluginPath.isEmpty())
		return;

	try
	{
		ctkPluginContext* pc = this->getPluginContext();
		pc->installPlugin(QUrl::fromLocalFile(pluginPath))->uninstall();
	}
	catch (const ctkPluginException& exc)
	{
		CX_LOG_CHANNEL_WARNING("plugin") << QString("Failed to uninstall plugin: %1, %2").arg(symbolicName).arg(exc.what());
		return;
	}

	return;
}

bool PluginFrameworkManager::start(const QString& symbolicName, ctkPlugin::StartOptions options)
{
	this->startFramework();

	QString pluginPath = getPluginPath(symbolicName);
	if (pluginPath.isEmpty())
	{
		CX_LOG_CHANNEL_ERROR("plugin") << QString("Failed to find plugin %1 in search path.").arg(symbolicName);
		return false;
	}

	try
	{
		ctkPluginContext* pc = this->getPluginContext();
		pc->installPlugin(QUrl::fromLocalFile(pluginPath))->start(options);
	}
	catch (const ctkPluginException& exc)
	{
		CX_LOG_CHANNEL_ERROR("plugin") << QString("Failed to start plugin (plugin error): %1, %2").arg(symbolicName).arg(exc.what());
		return false;
	}
	catch (const ctkRuntimeException& exc)
	{
		CX_LOG_CHANNEL_ERROR("plugin") << QString("Failed to start plugin (runtime error): %1, %2").arg(symbolicName).arg(exc.what());
		return false;
	}

	return true;
}

bool PluginFrameworkManager::stop(const QString& symbolicName, ctkPlugin::StopOptions options)
{
	if (!this->frameworkStarted())
		return false;
	QString pluginPath = this->getPluginPath(symbolicName);
	if (pluginPath.isEmpty())
		return false;

	QSharedPointer<ctkPlugin> plugin = this->getInstalledPluginFromSymbolicName(symbolicName);

	if (!plugin)
	{
		CX_LOG_CHANNEL_WARNING("plugin") << QString("Plugin: %1 not found").arg(symbolicName);
		return false;
	}

	try
	{
		plugin->stop(options);
	}
	catch (const ctkPluginException& exc)
	{
		CX_LOG_CHANNEL_WARNING("plugin") << QString("Failed to stop plugin %1: ").arg(symbolicName).arg(exc.what());
		return false;
	}

	return true;
}

QSharedPointer<ctkPlugin> PluginFrameworkManager::getInstalledPluginFromSymbolicName(QString symbolicName)
{
	QSharedPointer<ctkPlugin> empty;

	if (!this->frameworkInitialized())
		return empty;

	QString pluginPath = this->getPluginPath(symbolicName);
	if (pluginPath.isEmpty())
		return empty;

	ctkPluginContext* pc = this->getPluginContext();
	QList < QSharedPointer<ctkPlugin> > plugins = pc->getPlugins();
	foreach(QSharedPointer<ctkPlugin> plugin, plugins)
	{
		if (plugin->getSymbolicName() == symbolicName)
		{
			return plugin;
		}
	}

	return empty;
}


QString PluginFrameworkManager::getPluginPath(const QString& symbolicName)
{
	QString pluginFileName(symbolicName);
	pluginFileName.replace(".", "_");
	foreach(QString searchPath, mPluginSearchPaths)
	{
		QDirIterator dirIter(searchPath, mPluginLibFilter, QDir::Files, QDirIterator::Subdirectories);
		while(dirIter.hasNext())
		{
			dirIter.next();
			QFileInfo fileInfo = dirIter.fileInfo();
			QString fileBaseName = fileInfo.baseName();
			if (fileBaseName.startsWith("lib")) fileBaseName = fileBaseName.mid(3);

			if (fileBaseName == pluginFileName)
			{
				return fileInfo.canonicalFilePath();
			}
		}
	}

	return QString();
}

QStringList PluginFrameworkManager::getPluginSymbolicNames()
{
	QStringList result;
	foreach(QString searchPath, mPluginSearchPaths)
	{
		result.append(this->getPluginSymbolicNames(searchPath));
	}
	result.removeDuplicates();
	return result;
}

QStringList PluginFrameworkManager::getPluginSymbolicNames(const QString& searchPath)
{
	QStringList result;
	QDirIterator dirIter(searchPath, mPluginLibFilter, QDir::Files, QDirIterator::Subdirectories);
	while (dirIter.hasNext())
	{
		dirIter.next();
		QFileInfo fileInfo = dirIter.fileInfo();
		QString fileBaseName = fileInfo.baseName();
		if (fileBaseName.startsWith("lib"))
			fileBaseName = fileBaseName.mid(3);
		QString name = fileBaseName.replace("_", ".");
		if (this->nameIsProbablyPlugin(name))
			result << name;
	}

	return result;
}

bool PluginFrameworkManager::nameIsProbablyPlugin(QString name) const
{
	// heuristic check for plugin-ish name
	if (name.count(".")<2) // some libs contain a _, they generate too much spam in installed version
		return false;
	if (name.contains("cxtest"))
		return false;
	return true;

}

} /* namespace cx */
