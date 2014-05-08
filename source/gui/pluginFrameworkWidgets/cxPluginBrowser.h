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

#ifndef CTKPLUGINBROWSER_H
#define CTKPLUGINBROWSER_H

#include <QMainWindow>

#include <ctkPluginEvent.h>
#include <ctkPluginFrameworkEvent.h>
#include <ctkPlugin.h>
#include <ctkServiceEvent.h>
#include <QModelIndex>
#include "cxPluginFramework.h"
#include "cxMultiFileInputWidget.h"

class QTableView;
class ctkPluginFramework;

namespace cx
{
class ctkPluginTableModel;

class ctkPluginBrowser : public QMainWindow
{
  Q_OBJECT

public:

  ctkPluginBrowser(PluginFrameworkManagerPtr framework);
  virtual ~ctkPluginBrowser();

private Q_SLOTS:

  void pluginSelected(const QModelIndex& index);
  void resizePluginsTableView();

  void frameworkEvent(const ctkPluginFrameworkEvent& event);
  void pluginEvent(const ctkPluginEvent& event);
  void serviceEvent(const ctkServiceEvent& event);

  void startPlugin();
  void startPluginNow();
  void stopPlugin();
  void toggleInstallPlugin();
  void searchPathChanged();

private:

//  void closeEvent(QCloseEvent* closeEvent);
  void setupUi();
  PluginData getSelectedPlugin();

  void updatePluginToolbar();
  void startPlugin(ctkPlugin::StartOptions options);
  void wrapInDockWidget(QWidget* widget);


  PluginFrameworkManagerPtr framework;
  ctkPluginTableModel* mPluginTableModel;

  QAction* startPluginNowAction;
  QAction* startPluginAction;
  QAction* stopPluginAction;
  QAction* toggleInstallPluginAction;

  cx::MultiFileInputWidget* searchPathSelectWidget;

  QTableView* pluginsTableView;
  QToolBar* pluginToolBar;
  QTextEdit* mPluginInfoWidget;
  void updatePluginInfo();

  QString mSettingsMainWindowStateTag;
  QString mSettingsMainWindowGeometryTag;
  QMap<ctkPluginEvent::Type, QString> pluginEventTypeToString;

};

}


#endif // CTKPLUGINBROWSER_H
