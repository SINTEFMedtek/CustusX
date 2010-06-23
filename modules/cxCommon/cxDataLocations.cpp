/*
 * cxDataLocations.cpp
 *
 *  Created on: Jun 22, 2010
 *      Author: christiana
 */
#include "cxDataLocations.h"

#include <iostream>
#include <QApplication>
#include <QDir>
#include <QSettings>
//#include "sscTypeConversions.h"
#include "cxConfig.h"

namespace cx
{

QString DataLocations::getBundlePath()
{
#ifdef __APPLE__
  QString path(qApp->applicationDirPath()+"/../../..");
  return path;
#else
  QString path(qApp->applicationDirPath());
  return path;
#endif
}

QString DataLocations::getConfigPath()
{
  QString path = getBundlePath()+"/config";
  if (QDir(path).exists())
    return path;
  return CX_CONFIG_ROOT;
}

QString DataLocations::getShaderPath()
{
  QString path(qApp->applicationDirPath()+"/../Resources/shaders");
  if (QDir(path).exists())
    return path;
  return "";
}

QString DataLocations::getAppDataPath()
{
  QString path = getBundlePath()+"/config/appdata";
  if (!QDir(path).exists())
  {
//    std::cout << "did not find " << path << std::endl;
    path = qApp->applicationDirPath();
  }
  return path;
}

QSettingsPtr DataLocations::getSettings()
{
  QString path = getAppDataPath();
  //return QSettingsPtr(new QSettings());
  return QSettingsPtr(new QSettings(path+"/custus.ini", QSettings::IniFormat));
}

} // namespace cx
