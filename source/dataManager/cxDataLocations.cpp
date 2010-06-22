/*
 * cxDataLocations.cpp
 *
 *  Created on: Jun 22, 2010
 *      Author: christiana
 */

#include "cxDataLocations.h"
#include "cxConfig.h"

#include <QApplication>

namespace cx
{

QString DataLocations::getConfigPath() const
{
  QString path(qApp->applicationDirPath()+"/config/");
  if (QDir(path).exists())
    return path;
  return CX_CONFIG_DIR;
}

QSettingsPtr DataLocations::getSettings() const
{
  QString path(qApp->applicationDirPath()+"/config/");
  if (!QDir(path).exists())
    path = qApp->applicationDirPath();
  return QSettingsPtr(new QSettings());
  //return QSettingsPtr(new QSettings(path, QSettings::IniFormat));
}


} // namespace cx
