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

#ifndef CTKPLUGINTABLEMODEL_H
#define CTKPLUGINTABLEMODEL_H

#include <QAbstractTableModel>

#include <QList>

#include <ctkPlugin.h>
#include <ctkPluginEvent.h>
#include "cxPluginFramework.h"

class ctkPluginContext;

namespace cx
{

class ctkPluginTableModel : public QAbstractTableModel
{

  Q_OBJECT

public:

  ctkPluginTableModel(PluginFrameworkManagerPtr framework, QObject* parent = 0);

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;

protected Q_SLOTS:

  void pluginChanged(const ctkPluginEvent& event);
  void resetAll();

private:
	std::map<QString, ctkPluginPtr> mPlugins;
  PluginFrameworkManagerPtr mFramework;
};

}

#endif // CTKPLUGINTABLEMODEL_H
