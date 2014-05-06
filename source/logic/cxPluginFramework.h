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

#ifndef CXPLUGINFRAMEWORK_H_
#define CXPLUGINFRAMEWORK_H_

#include <QString>
#include <QObject>
#include <boost/shared_ptr.hpp>

#include "ctkPlugin.h"
#include "ctkPluginFramework_global.h"

typedef QSharedPointer<ctkPlugin> ctkPluginPtr;
typedef std::pair<QString,ctkPluginPtr> PluginData;

class ctkPluginFramework;
class ctkPluginFrameworkFactory;

namespace cx
{
typedef boost::shared_ptr<class PluginFrameworkManager> PluginFrameworkManagerPtr;

/** Manages a ctkPluginFramework instance.
 *
 * This is a customized version of the ctk singleton ctkPluginFrameworkLauncher.
 *
 */
class PluginFrameworkManager : public QObject
{
	Q_OBJECT
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

	void addSearchPath(const QString& searchPath);
	void setSearchPaths(const QStringList& searchPath);
	QStringList getSearchPaths() const;
	QStringList getPluginSymbolicNames();
	QSharedPointer<ctkPlugin> getInstalledPluginFromSymbolicName(QString symbolicName);
	ctkPlugin::State getStateFromSymbolicName(QString name);

signals:
	void pluginPoolChanged();

private:
	QString getPluginPath(const QString& symbolicName);
	QStringList getPluginSymbolicNames(const QString& searchPath);
	bool nameIsProbablyPlugin(QString name) const;

	QScopedPointer<ctkPluginFrameworkFactory> mFrameworkFactory;
	QSharedPointer<ctkPluginFramework> mFramework;

	void initializeFramework();
	void startFramework();

	bool frameworkInitialized() const;
	bool frameworkStarted() const;

	void loadState();
	void saveState();
	void loadPluginFromStoredState(QString symbolicName, QString storedState);

	QStringList mPluginSearchPaths;
	QStringList mPluginLibFilter;

	QString mSettingsSearchPaths;
	QString mSettingsBase;

};

} /* namespace cx */
#endif /* CXPLUGINFRAMEWORK_H_ */
