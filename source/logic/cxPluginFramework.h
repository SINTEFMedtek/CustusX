/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLUGINFRAMEWORK_H_
#define CXPLUGINFRAMEWORK_H_

#include "cxLogicManagerExport.h"

#include <QString>
#include <QObject>
#include <boost/shared_ptr.hpp>

#include "ctkPlugin.h"
#include "ctkPluginFramework_global.h"

typedef QSharedPointer<ctkPlugin> ctkPluginPtr;
typedef std::pair<QString,ctkPluginPtr> PluginData;

class ctkPluginFramework;
class ctkPluginFrameworkFactory;
class ctkException;

namespace cx
{
typedef boost::shared_ptr<class PluginFrameworkManager> PluginFrameworkManagerPtr;

/** Manages a ctkPluginFramework instance.
 *
 * This is a customized version of the ctk singleton ctkPluginFrameworkLauncher.
 *
 */
class cxLogicManager_EXPORT PluginFrameworkManager : public QObject
{
	Q_OBJECT

	struct PluginLoadInfo
	{
		QString symbolicName;
		QString storedState;
		ctkPlugin::State targetState;
		bool isNew;
	};

public:
	static PluginFrameworkManagerPtr create() { return PluginFrameworkManagerPtr(new PluginFrameworkManager()); }

	PluginFrameworkManager();
	~PluginFrameworkManager();

	void install(const QString& symbolicName);
	void uninstall(const QString& symbolicName);
	bool start(const QString& symbolicName, ctkPlugin::StartOptions options = ctkPlugin::START_ACTIVATION_POLICY);
	bool stop(const QString& symbolicName, ctkPlugin::StopOptions options = 0);
	bool start();
	bool stop();

	ctkPluginContext* getPluginContext();
	QSharedPointer<ctkPluginFramework> getPluginFramework();

//	void addSearchPath(const QString& searchPath);
	void setSearchPaths(const QStringList& searchPath);
	QStringList getSearchPaths() const;
	QStringList getPluginSymbolicNames();
	QSharedPointer<ctkPlugin> getInstalledPluginFromSymbolicName(QString symbolicName);
	ctkPlugin::State getStateFromSymbolicName(QString name);
	void loadState();

signals:
	void pluginPoolChanged();
    void aboutToStop();

private:
	QString getPluginPath(const QString& symbolicName);
	QStringList getPluginSymbolicNames(const QString& searchPath);
	bool nameIsProbablyPlugin(QString name) const;
	std::vector<PluginFrameworkManager::PluginLoadInfo> getPluginLoadInfo(QStringList symbolicNames);

	QScopedPointer<ctkPluginFrameworkFactory> mFrameworkFactory;
	QSharedPointer<ctkPluginFramework> mFramework;

	void initializeFramework();
	void startFramework();

	bool frameworkInitialized() const;
	bool frameworkStarted() const;

	void saveState();
	void loadPluginFromStoredState(QString symbolicName, QString storedState);

	QString convertToRelativePath(QString path) const;
	QString convertToAbsolutePath(QString path) const;

	QStringList mPluginSearchPaths;
	QStringList mPluginLibFilter;

	QString mSettingsSearchPaths;
	QString mSettingsBase;

	void handlePluginException(const QString& message, const ctkException &exc);
};

} /* namespace cx */
#endif /* CXPLUGINFRAMEWORK_H_ */
