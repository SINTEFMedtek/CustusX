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
