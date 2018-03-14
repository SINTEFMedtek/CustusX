/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	QStringList paths = settings()->value(mSettingsSearchPaths, QStringList()).toStringList();
	this->setSearchPaths(paths);

	QStringList names = this->getPluginSymbolicNames();
	std::vector<PluginLoadInfo> info = this->getPluginLoadInfo(names);

	// install all plugins, must do this first in order to let FW handle dependencies.
	CX_LOG_CHANNEL_INFO("plugin") << "Installing all plugins...";
	for (unsigned i=0; i< info.size(); ++i)
	{
		if (info[i].targetState != ctkPlugin::UNINSTALLED)
        {
			this->install(info[i].symbolicName);
        }
	}

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
    {
		return;
    }

	QSharedPointer<ctkPluginFramework> framework = mFrameworkFactory->getFramework();

	try
	{
		framework->init();
	} catch (const ctkException& exc)
	{
		this->handlePluginException("Failed to initialize the plug-in framework", exc);
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
	catch (const ctkException& exc)
	{
		this->handlePluginException("Failed to start the plug-in framework", exc);
	}
}

void PluginFrameworkManager::install(const QString& symbolicName)
{
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
	catch (const ctkException& exc)
	{
		this->handlePluginException(QString("Failed to install plugin %1").arg(symbolicName), exc);
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
	catch (const ctkException& exc)
	{
		this->handlePluginException("Failed to stop the plug-in framework", exc);
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
	catch (const ctkException& exc)
	{
		this->handlePluginException(QString("Failed to uninstall plugin %1.").arg(symbolicName), exc);
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
		QSharedPointer<ctkPlugin> plugin = pc->installPlugin(QUrl::fromLocalFile(pluginPath));
		plugin->start(options);
	}
	catch (ctkException& exc)
	{
		this->handlePluginException(QString("Failed to stop plugin %1.").arg(symbolicName), exc);
		return false;
	}

	return true;
}

void PluginFrameworkManager::handlePluginException(const QString& message, const ctkException& exc)
{
	CX_LOG_CHANNEL_ERROR("plugin") << message;
	const ctkException* nest = &exc;
	while (nest)
	{
		CX_LOG_CHANNEL_ERROR("plugin") << QString("  cause: %1").arg(nest->what());
		nest = nest->cause();
	}
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
	catch (const ctkException& exc)
	{
		this->handlePluginException(QString("Failed to stop plugin %1").arg(symbolicName), exc);
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
	return name.startsWith("org.custusx");
}

} /* namespace cx */
