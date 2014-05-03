// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
#include "cxReporter.h"

namespace cx
{

PluginFrameworkManager::PluginFrameworkManager()
{
	mSettingsBase = "pluginFramework";
	mSettingsSearchPaths = mSettingsBase + "/searchPaths";

	ctkProperties fwProps;
	QString storagePath = DataLocations::getSettingsPath() + "/pluginFramework";
	fwProps[ctkPluginConstants::FRAMEWORK_STORAGE] = storagePath;
	//	  fwProps[ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN] = ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT;

	mFrameworkFactory.reset(new ctkPluginFrameworkFactory(fwProps));
	mPluginLibFilter << "*.dll" << "*.so" << "*.dylib";
	this->loadState();
}

PluginFrameworkManager::~PluginFrameworkManager()
{
	this->saveState();
}

void PluginFrameworkManager::loadState()
{
	QStringList defPaths(DataLocations::getDefaultPluginsPath());
	qDebug() << "defPaths: " << defPaths;
	QStringList paths = settings()->value(mSettingsSearchPaths, defPaths).toStringList();
	qDebug() << "paths: " << paths;
	if (!paths.isEmpty())
		this->setSearchPaths(paths);

	QStringList names = this->getPluginSymbolicNames();
	for (unsigned i=0; i<names.size(); ++i)
	{
		QString name = names[i];
		QString storedState = settings()->value(mSettingsBase+"/"+name).toString();
		this->loadPluginFromStoredState(name, storedState);
	}
}

void PluginFrameworkManager::loadPluginFromStoredState(QString symbolicName, QString storedState)
{
	if (storedState.isEmpty())
	{
		report(QString("Detected new plugin [%1], autostarting...").arg(symbolicName));
		storedState = getStringForctkPluginState(ctkPlugin::ACTIVE);
	}

	ctkPlugin::State state = getctkPluginStateForString(storedState);

	if (state==ctkPlugin::UNINSTALLED)
		return;

	if (state==ctkPlugin::ACTIVE)
		this->start(symbolicName, ctkPlugin::START_TRANSIENT);
	else
		this->install(symbolicName);
}

void PluginFrameworkManager::saveState()
{
	settings()->setValue(mSettingsSearchPaths, this->getSearchPaths());

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

void PluginFrameworkManager::addSearchPath(const QString& searchPath)
{
	mPluginSearchPaths << searchPath;
	QApplication::addLibraryPath(searchPath);
}

void PluginFrameworkManager::setSearchPaths(const QStringList& searchPath)
{
	mPluginSearchPaths = searchPath;
	for (int i=0; i<searchPath.size(); ++i)
		QApplication::addLibraryPath(searchPath[i]);
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
		qWarning() << "Failed to install plugin:" << symbolicName << ", " << exc;
	}
}

bool PluginFrameworkManager::start()
{
	this->startFramework();
	return this->frameworkStarted();
}

bool PluginFrameworkManager::stop()
{
	ctkPluginContext* pc = this->getPluginContext();
    // stop the framework
    QSharedPointer<ctkPluginFramework> fw = qSharedPointerCast<ctkPluginFramework>(pc->getPlugin(0));
    try
    {
      fw->stop();
      ctkPluginFrameworkEvent fe = fw->waitForStop(5000);
      if (fe.getType() == ctkPluginFrameworkEvent::FRAMEWORK_WAIT_TIMEDOUT)
      {
        qWarning() << "Stopping the plugin framework timed out";
        return false;
      }
    }
    catch (const ctkRuntimeException& e)
    {
      qWarning() << "Stopping the plugin framework failed: " << e;
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
		qWarning() << "Failed to uninstall plugin:" << symbolicName << ", " << exc;
		return;
	}

	return;
}

bool PluginFrameworkManager::start(const QString& symbolicName, ctkPlugin::StartOptions options)
{
	this->startFramework();

	QString pluginPath = getPluginPath(symbolicName);
	if (pluginPath.isEmpty())
		return false;

	try
	{
		ctkPluginContext* pc = this->getPluginContext();
		pc->installPlugin(QUrl::fromLocalFile(pluginPath))->start(options);
	}
	catch (const ctkPluginException& exc)
	{
		qWarning() << "Failed to start plugin:" << symbolicName << ", " << exc;
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
		qWarning() << "Plug-in" << symbolicName << "not found";
		return false;
	}

	try
	{
		plugin->stop(options);
	}
	catch (const ctkPluginException& exc)
	{
		qWarning() << "Failed to stop plug-in:" << exc;
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
		if (name.contains(".")) // heuristic check for plugin-ish name
			result << name;
	}

	return result;
}

} /* namespace cx */
