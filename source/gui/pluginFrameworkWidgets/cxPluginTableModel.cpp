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

#include "cxPluginTableModel.h"

#include <QtDebug>
#include <ctkPlugin.h>
#include <ctkPluginContext.h>
#include <iostream>

#include "cxPluginFrameworkUtilities.h"

namespace cx
{

ctkPluginTableModel::ctkPluginTableModel(PluginFrameworkManagerPtr framework, QObject* parent)
  : QAbstractTableModel(parent)
{
	mFramework = framework;
	connect(mFramework.get(), SIGNAL(pluginPoolChanged()), this, SLOT(resetAll()));
	mFramework->getPluginContext()->connectPluginListener(this, SLOT(pluginChanged(ctkPluginEvent)));
	this->resetAll();
}

void ctkPluginTableModel::resetAll()
{
	this->beginResetModel();
	QStringList names = mFramework->getPluginSymbolicNames();
	std::map<QString, ctkPluginPtr> plugins;
	for (int i=0; i<names.size(); ++i)
		plugins[names[i]].clear();

	QList<ctkPluginPtr> installed = mFramework->getPluginContext()->getPlugins();
	for (int i=0; i<installed.size(); ++i)
		plugins[installed[i]->getSymbolicName()] = installed[i];

	mPlugins = plugins;

	this->endResetModel();
}

QVariant ctkPluginTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) return QVariant();

	std::map<QString, ctkPluginPtr>::const_iterator iter = mPlugins.begin();
	std::advance(iter, index.row());
	QString name = iter->first;
	QSharedPointer<ctkPlugin> plugin = iter->second;

  if (role == Qt::DisplayRole)
  {
    int col = index.column();
    if (col == 0)
    {
    	return QVariant(name);
    }
    else if (col == 1)
    {
    	if (plugin)
    		return QVariant(plugin->getVersion().toString());
    }
    else if (col == 2)
    {
    	if (plugin)
    		return QVariant(getStringForctkPluginState(plugin->getState()));
    	else
    		return getStringForctkPluginState(ctkPlugin::UNINSTALLED);
    }
  }
  else if (role == Qt::UserRole)
  {
  	return QVariant(name);
  }

  return QVariant();
}

QVariant ctkPluginTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    if (section == 0)
    {
      return QVariant("Plugin");
    }
    else if (section == 1)
    {
      return QVariant("Version");
    }
    else if (section == 2)
    {
      return QVariant("State");
    }
  }

  return QVariant();
}

int ctkPluginTableModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)

  return 3;
}

int ctkPluginTableModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)

		return mPlugins.size();
}

void ctkPluginTableModel::pluginChanged(const ctkPluginEvent& event)
{
	if (!event.getPlugin())
	{
		qDebug() << "!! invalid plugin event";
		return;
	}

	int i=0;
	for(std::map<QString, ctkPluginPtr>::iterator iter=mPlugins.begin(); iter!=mPlugins.end(); ++iter, ++i)
	{
		if (event.getPlugin()->getSymbolicName() != iter->first)
			continue;

		iter->second = event.getPlugin();

		QModelIndex topLeftIndex = createIndex(i, 0);
		QModelIndex bottomRightIndex = createIndex(topLeftIndex.row(), columnCount()-1);
		emit dataChanged(topLeftIndex, bottomRightIndex);
	}
}

}
