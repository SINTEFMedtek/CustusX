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
#include "cxConfig.h"

namespace cx
{

QString DataLocations::getTestDataPath()
{
  return CX_DATA_ROOT;
}

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

QString DataLocations::getRootConfigPath()
{
  QString path = getBundlePath()+"/config";
  if (QDir(path).exists())
    return path;
  return CX_CONFIG_ROOT;
}

QString DataLocations::getToolsPath()
{
  QString path(getRootConfigPath()+"/tool/Tools/");
  return path;
}
  
QString DataLocations::getApplicationToolConfigPath()
{
  QString path(getRootConfigPath()+"/tool/" +
               getSettings()->value("globalApplicationName").toString());
  //std::cout << "getApplicationToolConfigPath: " << path.toStdString() << std::endl;
  return path;
}
  
QString DataLocations::getToolConfigFilePath()
{
  QString path(getRootConfigPath()+"/tool/" +
               getSettings()->value("globalApplicationName").toString() + "/" +
               getSettings()->value("toolConfigFile").toString());
  //std::cout << "getToolConfigFilePath: " << path.toStdString() << std::endl;
  return path;
}

QString DataLocations::getAudioConfigFilePath()
{
  QString path(getRootConfigPath()+"/audio/");
  return path;
}
  
QString DataLocations::getShaderPath()
{
//  QString path(qApp->applicationDirPath()+"/../Resources/shaders");
  QString path = getRootConfigPath()+"/shaders";
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

/** return a settings object for global custusX data
 *
 *  The settings object is currently stored in the default location for the OS.
 */
QSettingsPtr DataLocations::getSettings()
{
  return QSettingsPtr(new QSettings());

  //QString path = getAppDataPath();
  //return QSettingsPtr(new QSettings(path+"/custus.ini", QSettings::IniFormat));
}

namespace
{
QString changeExtension(QString name, QString ext)
{
  QStringList splitName = name.split(".");
  splitName[splitName.size()-1] = ext;
  return splitName.join(".");
}
}

QString DataLocations::getXmlSettingsFile()
{
  return changeExtension(getSettings()->fileName(), "xml");
//  return getAppDataPath() + "/CustusX.xml";
}


} // namespace cx
