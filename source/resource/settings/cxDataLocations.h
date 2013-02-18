/*
 * cxDataLocations.h
 *
 *  \date Jun 22, 2010
 *      \author christiana
 */
#ifndef CXDATALOCATIONS_H_
#define CXDATALOCATIONS_H_

#include <QString>
#include "boost/shared_ptr.hpp"

namespace cx
{

/**Helper functions for accessing file paths and similar stuff.
 * \ingroup cxResourceSettings
 *
 */
class DataLocations
{
public:
  static QString getBundlePath(); ///< return the folder where the bundle or executable are located.
  static QString getRootConfigPath(); ///< return path to root config folder
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
#ifdef CX_USE_TSF
  static QString getTSFPath(); ///< return path to a folder where files needed for tube segmentation framework are located
#endif

  static void setTestMode(); ///< set a testing mode that changes location of settings files to a temp folder.

private:
  static bool mTestMode;
//  static DataLocations* mInstance;
//  static DataLocations getInstance();

};

} // namespace cx

#endif /* CXDATALOCATIONS_H_ */
