/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

// Based on a class from CTK:

/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "cxPluginBrowser.h"
#include <QtWidgets>

#include "cxPluginTableModel.h"
#include "ctkServiceReference.h"
#include "ctkServiceException.h"
#include "ctkPluginConstants.h"

#include <ctkConfig.h>
#include <ctkPluginException.h>
#include <ctkPluginFramework.h>
#include <ctkPluginContext.h>
#include <iostream>

#include <QApplication>
#include <QMainWindow>
#include <QStringList>
#include <QDirIterator>
#include <QUrl>
#include <QSettings>
#include <QCloseEvent>
#include "cxSettings.h"

//#define SETTINGS_WND_GEOM "mainwindow.geom"
//#define SETTINGS_WND_STATE "mainwindow.state"

namespace cx
{

ctkPluginBrowser::ctkPluginBrowser(PluginFrameworkManagerPtr framework)
: framework(framework)
{
	mSettingsMainWindowStateTag = "pluginbrowser/state";
	mSettingsMainWindowGeometryTag = "pluginbrowser/geometry";

	pluginEventTypeToString[ctkPluginEvent::INSTALLED] = "Installed";
	pluginEventTypeToString[ctkPluginEvent::LAZY_ACTIVATION] = "Lazy Activation";
	pluginEventTypeToString[ctkPluginEvent::RESOLVED] = "Resolved";
	pluginEventTypeToString[ctkPluginEvent::STARTED] = "Started";
	pluginEventTypeToString[ctkPluginEvent::STARTING] = "Starting";
	pluginEventTypeToString[ctkPluginEvent::STOPPED] = "Stopped";
	pluginEventTypeToString[ctkPluginEvent::STOPPING] = "Stopping";
	pluginEventTypeToString[ctkPluginEvent::UNINSTALLED] = "Uninstalled";
	pluginEventTypeToString[ctkPluginEvent::UNRESOLVED] = "Unresolved";
	pluginEventTypeToString[ctkPluginEvent::UPDATED] = "Updated";

//	qDebug() << "ctkPluginBrowser::ctkPluginBrowser 1";
//	qDebug() << "ctkPluginBrowser::ctkPluginBrowser fw is null: " << (framework.get() == NULL);
//	qDebug() << "ctkPluginBrowser::ctkPluginBrowser" << framework->getPluginContext();
	framework->getPluginContext()->connectFrameworkListener(this, SLOT(frameworkEvent(ctkPluginFrameworkEvent)));
	framework->getPluginContext()->connectPluginListener(this, SLOT(pluginEvent(ctkPluginEvent)));
	framework->getPluginContext()->connectServiceListener(this, "serviceEvent");
//	qDebug() << "ctkPluginBrowser::ctkPluginBrowser 2";

	this->setupUi();

	mPluginTableModel = new ctkPluginTableModel(framework, this);
	pluginsTableView->setModel(mPluginTableModel);

	connect(pluginsTableView->selectionModel(),
			&QItemSelectionModel::currentChanged,
			this,
			&ctkPluginBrowser::pluginSelected);

	startPluginNowAction = new QAction(QIcon(":/icons/open_icon_library/arrow-right-double-3.png"), "Start Plugin (ignore activation policy)", this);
	startPluginAction = new QAction(QIcon(":/icons/open_icon_library/arrow-right-3.png"), "Start Plugin", this);
	stopPluginAction = new QAction(QIcon(":/icons/open_icon_library/media-record-3.png"), "Stop Plugin", this);
	toggleInstallPluginAction = new QAction(QIcon(":/icons/preset_reset.png"), "Toggle Install", this);

	connect(startPluginNowAction, SIGNAL(triggered()), this, SLOT(startPluginNow()));
	connect(startPluginAction, SIGNAL(triggered()), this, SLOT(startPlugin()));
	connect(stopPluginAction, SIGNAL(triggered()), this, SLOT(stopPlugin()));
	connect(toggleInstallPluginAction, SIGNAL(triggered()), this, SLOT(toggleInstallPlugin()));

	startPluginNowAction->setEnabled(false);
	startPluginAction->setEnabled(false);
	stopPluginAction->setEnabled(false);

	pluginToolBar->addAction(startPluginNowAction);
	pluginToolBar->addAction(startPluginAction);
	pluginToolBar->addAction(stopPluginAction);
	pluginToolBar->addAction(toggleInstallPluginAction);

	  if(settings()->contains(mSettingsMainWindowGeometryTag))
	  {
	    this->restoreGeometry(settings()->value(mSettingsMainWindowGeometryTag).toByteArray());
	  }
	  if (settings()->contains(mSettingsMainWindowStateTag))
	  {
	    this->restoreState(settings()->value(mSettingsMainWindowStateTag).toByteArray());
	  }

	QTimer::singleShot(0, this, SLOT(resizePluginsTableView()));
}

ctkPluginBrowser::~ctkPluginBrowser()
{
	settings()->setValue(mSettingsMainWindowGeometryTag, this->saveGeometry());
	settings()->setValue(mSettingsMainWindowStateTag, this->saveState());
}

void ctkPluginBrowser::resizePluginsTableView()
{
	pluginsTableView->resizeColumnToContents(0);
}

void ctkPluginBrowser::setupUi()
{
	pluginsTableView = new QTableView(this);
	pluginsTableView->setObjectName(QString::fromUtf8("pluginsTableView"));
	pluginsTableView->setWindowTitle("Plugins");
	pluginsTableView->setAutoScroll(false);
	pluginsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	pluginsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	pluginsTableView->setShowGrid(false);
	pluginsTableView->setSortingEnabled(true);
	pluginsTableView->horizontalHeader()->setCascadingSectionResizes(true);
	pluginsTableView->horizontalHeader()->setHighlightSections(false);
	pluginsTableView->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
	pluginsTableView->horizontalHeader()->setStretchLastSection(true);
	pluginsTableView->verticalHeader()->setVisible(false);

	this->wrapInDockWidget(pluginsTableView);

	pluginToolBar = new QToolBar(this);
	pluginToolBar->setObjectName(QString::fromUtf8("pluginToolBar"));
	this->addToolBar(Qt::TopToolBarArea, pluginToolBar);

	searchPathSelectWidget = new cx::MultiFileInputWidget(this);
	searchPathSelectWidget->setDescription("Search Paths");
	searchPathSelectWidget->setWindowTitle("Search Paths");

	searchPathSelectWidget->setHelp("Select where to look for plugins");
	searchPathSelectWidget->setBrowseHelp("Browse for a search path");
	searchPathSelectWidget->setUseRelativePath(true);
	searchPathSelectWidget->setBasePath(qApp->applicationDirPath());

	searchPathSelectWidget->setFilenames(framework->getSearchPaths());
	connect(searchPathSelectWidget, SIGNAL(fileChanged()), this, SLOT(searchPathChanged()));

	this->wrapInDockWidget(searchPathSelectWidget);

	mPluginInfoWidget = new QTextEdit(this);
	mPluginInfoWidget->setObjectName("PluginInfoWidget");
	mPluginInfoWidget->setWindowTitle("Plugin Info");
	this->wrapInDockWidget(mPluginInfoWidget);

}

void ctkPluginBrowser::wrapInDockWidget(QWidget* widget)
{
	QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), this);
	dockWidget->setObjectName(widget->objectName()+"_dockwidget");
	dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
	dockWidget->setWidget(widget);
	this->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);
}

void ctkPluginBrowser::searchPathChanged()
{
	QStringList files = searchPathSelectWidget->getAbsoluteFilenames();
	QStringList validFiles;
	for (int i=0; i<files.size(); ++i)
		if (QFileInfo(files[i]).exists())
			validFiles << files[i];

	framework->setSearchPaths(validFiles);
}

void ctkPluginBrowser::pluginSelected(const QModelIndex &index)
{
	this->updatePluginToolbar();
	this->updatePluginInfo();
}

void ctkPluginBrowser::updatePluginToolbar()
{
	startPluginNowAction->setEnabled(false);
	startPluginAction->setEnabled(false);
	stopPluginAction->setEnabled(false);

	PluginData plugin = this->getSelectedPlugin();

	if (!plugin.second)
	{
		toggleInstallPluginAction->setText("Install");
		return;
	}
	toggleInstallPluginAction->setText("Uninstall");

	const ctkPlugin::States startStates = ctkPlugin::INSTALLED | ctkPlugin::RESOLVED | ctkPlugin::STOPPING;
	const ctkPlugin::States stopStates = ctkPlugin::STARTING | ctkPlugin::ACTIVE;
	if (startStates.testFlag(plugin.second->getState()))
	{
		startPluginNowAction->setEnabled(true);
		startPluginAction->setEnabled(true);
	}

	if (stopStates.testFlag(plugin.second->getState()))
	{
		stopPluginAction->setEnabled(true);
	}
}

void ctkPluginBrowser::updatePluginInfo()
{
	PluginData plugin = this->getSelectedPlugin();

	QString text;

	if (plugin.second)
	{
		QString manifest("/META-INF/MANIFEST.MF");
		text += QString("%1").arg(manifest);
		text += QString("<hr>");
		QByteArray mfContent = plugin.second->getResource(manifest);
		QString rawText("<div style=\"white-space: pre-wrap;\">%1</div>");
		text += rawText.arg(QString(mfContent));
		text += QString("<hr>");
		text += QString("<p><b>Location:</b> %1</p>").arg(plugin.second->getLocation());
	}

	if (!plugin.second.isNull() && plugin.second->getPluginContext())
	{
		QStringList serviceTexts;
		QList<ctkServiceReference> serviceRefs = plugin.second->getPluginContext()->getServiceReferences("");
		QListIterator<ctkServiceReference> it(serviceRefs);
		while (it.hasNext())
		{
			ctkServiceReference ref(it.next());

			if (ref.getPlugin()->getSymbolicName() != plugin.first)
				continue;

			serviceTexts += QString("<tr><td><b>Service</td></tr>");
			QStringList propKeys = ref.getPropertyKeys();
			for (int i=0; i<propKeys.size(); ++i)
			{
				QString val = ref.getProperty(propKeys[i]).toString();
				QString bblue("<b><font color=\"blue\">%1</font></b>");
				QString green("<b><font color=\"green\">%1</font></b>");
				QString formattedKey = green.arg(propKeys[i]);
				QString formattedVal = bblue.arg(val);
				serviceTexts += QString("<tr> <td></td> <td>%1</td> <td>%2</td>  </tr>").arg(formattedKey).arg(formattedVal);
			}
		}

		text += QString("<table>%1</table>").arg(serviceTexts.join("\n"));
	}
	else
	{
		text += QString("<p>Service info not available</p>");
	}

	mPluginInfoWidget->setHtml(text);
}

void ctkPluginBrowser::frameworkEvent(const ctkPluginFrameworkEvent& event)
{
	qDebug() << "FrameworkEvent: [" << event.getPlugin()->getSymbolicName() << "]" << event.getErrorString();
}

void ctkPluginBrowser::pluginEvent(const ctkPluginEvent& event)
{
	qDebug() << "PluginEvent: [" << event.getPlugin()->getSymbolicName() << "]" << pluginEventTypeToString[event.getType()];

	updatePluginToolbar();
}

void ctkPluginBrowser::serviceEvent(const ctkServiceEvent &event)
{
	ctkServiceReference ref = event.getServiceReference();
	qDebug() << "ServiceEvent: [" << event.getType() << "]" << event.getServiceReference().getUsingPlugins();
	qDebug() << "  keys: " << ref.getPropertyKeys();
	qDebug() << "  objectclass: " << ref.getProperty("objectclass");
}

PluginData ctkPluginBrowser::getSelectedPlugin()
{
	QModelIndex selection = pluginsTableView->selectionModel()->currentIndex();
	QVariant v = selection.data(Qt::UserRole);
	QString name = v.toString();
	ctkPluginPtr plugin = framework->getInstalledPluginFromSymbolicName(name);
	return std::make_pair(name, plugin);
}

void ctkPluginBrowser::toggleInstallPlugin()
{
	PluginData plugin = this->getSelectedPlugin();
	if (plugin.second)
	{
		framework->uninstall(plugin.first);
	}
	else
	{
		framework->install(plugin.first);
	}
}

void ctkPluginBrowser::startPlugin()
{
	startPlugin(ctkPlugin::START_TRANSIENT | ctkPlugin::START_ACTIVATION_POLICY);
}

void ctkPluginBrowser::startPluginNow()
{
	startPlugin(ctkPlugin::START_TRANSIENT);
}

void ctkPluginBrowser::startPlugin(ctkPlugin::StartOptions options)
{
	PluginData plugin = this->getSelectedPlugin();
	framework->start(plugin.first, options);
}

void ctkPluginBrowser::stopPlugin()
{
	PluginData plugin = this->getSelectedPlugin();
	framework->stop(plugin.first);
}

}
