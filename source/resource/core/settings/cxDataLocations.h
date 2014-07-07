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
#ifndef CXDATALOCATIONS_H_
#define CXDATALOCATIONS_H_

#include <QString>

namespace cx
{

/**Helper functions for accessing file paths and similar stuff.
 * \ingroup cx_resource_core_settings
 *  \date Jun 22, 2010
 *  \author christiana
 *
 */
class DataLocations
{
public:
  static QString getBundlePath(); ///< return the folder where the bundle or executable are located.
	static QString getRootConfigPath(); ///< return path to root config folder. May be replaced with getExistingConfigPath()
  static QString getTestDataPath(); ///< return path to test data folder
  static QString getToolsPath(); ///< return path to folder containing all defined tools
  static QString getApplicationToolConfigPath(); ///< return path to tool config folder, based on choosen application
  static QString getToolConfigFilePath(); ///< return users path to tool config file
  static QString getAudioConfigFilePath(); ///< return users path to audio config file
  static QString getShaderPath(); ///< return the path to installed shaders. empty string if not installed.
  static QString getAppDataPath(); ///< return path to global configuration data.
  static QString getSettingsPath(); ///< return path where settings are to be stored.
  static QString getXmlSettingsFile(); ///< return a filename for a xml file containing global custusX data (equal to settings above)
	static QString getCachePath(); ///< return path to a folder that is used during execution, will be cleared at start and stop.
	static QStringList getDefaultPluginsPath(); ///< return the folder where plugins should be located, by default.

	static QString getExistingConfigPath(QString pathRelativeToConfigRoot, QString alternativeAbsolutePath); ///< Return folder path. Path is typically either relative to config root (created during installation), or an alternative absolute path (typically in the source tree)

  static void setTestMode(); ///< set a testing mode that changes location of settings files to a temp folder.

private:
  static bool mTestMode;
  static QString readTestDataPathFromFile(QString filename);
//  static DataLocations* mInstance;
//  static DataLocations getInstance();

};

} // namespace cx

#endif /* CXDATALOCATIONS_H_ */
