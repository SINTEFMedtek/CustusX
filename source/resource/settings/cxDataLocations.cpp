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
#include "cxDataLocations.h"

#include <iostream>
#include <QApplication>
#include <QDir>
#include "cxConfig.h"
#include "cxSettings.h"
#include "sscTypeConversions.h"

#ifdef CX_USE_TSF
#include "tsf-config.h"
#endif

namespace cx
{

//---------------------------------------------------------
bool DataLocations::mTestMode = false;
//---------------------------------------------------------

void DataLocations::setTestMode()
{
	mTestMode = true;
}

QString DataLocations::getTestDataPath()
{
  return CX_DATA_ROOT;
}

QString DataLocations::getSettingsPath()
{
	QString retval = cx::DataLocations::getRootConfigPath() + "/settings";
	if (mTestMode)
		retval = getTestDataPath() + "/temp/settings";
	return retval;
}

QString DataLocations::getBundlePath()
{
#ifdef __APPLE__
  QString path(qApp->applicationDirPath()+"/../../..");
  QString bundle(qApp->applicationDirPath()+"/../..");
//  std::cout << "check bundle: " << bundle << ", isbundle=" << QFileInfo(bundle).isBundle() << std::endl;
  if (QFileInfo(bundle).isBundle())
	  return path;
  else
	  return qApp->applicationDirPath();
#else
  QString path(qApp->applicationDirPath());
  return path;
#endif
}

QString DataLocations::getRootConfigPath()
{
  QString path = getBundlePath() + "/" + CX_CONFIG_ROOT_RELATIVE_INSTALLED; // look for installed location
  if (QDir(path).exists())
    return path;

  if (QDir(CX_CONFIG_ROOT).exists()) // look for folder in source code
    return CX_CONFIG_ROOT;

  return "";
}

QString DataLocations::getToolsPath()
{
  QString path(getRootConfigPath()+"/tool/Tools/");
  return path;
}
  
QString DataLocations::getApplicationToolConfigPath()
{
  QString path(getRootConfigPath()+"/tool/" +
               settings()->value("globalApplicationName").toString());
  //std::cout << "getApplicationToolConfigPath: " << path.toStdString() << std::endl;
  return path;
}
  
QString DataLocations::getToolConfigFilePath()
{
  QString path(getRootConfigPath()+"/tool/" +
               settings()->value("globalApplicationName").toString() + "/" +
               settings()->value("toolConfigFile").toString());
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
	return getSettingsPath() + "/settings.xml";
//  return changeExtension(settings()->fileName(), "xml");
//  return getAppDataPath() + "/CustusX.xml";
}

QString DataLocations::getCachePath()
{
	QString path(getRootConfigPath()+"/cache/");
    return path;
}

#ifdef CX_USE_TSF
QString DataLocations::getTSFPath()
{
	QString path = getBundlePath() + "/" + CX_CONFIG_ROOT_RELATIVE_INSTALLED + "/tsf"; // look for installed location
	if (QDir(path).exists())
		return path;

	if (QDir(KERNELS_DIR).exists()) // look for folder in source code
		return KERNELS_DIR;

	return "";
}
#endif



} // namespace cx
