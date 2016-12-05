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
