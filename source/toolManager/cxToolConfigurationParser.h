#ifndef CXTOOLCONFIGURATIONPARSER_H_
#define CXTOOLCONFIGURATIONPARSER_H_

#include <QString>
#include <QDomDocument>
#include "sscToolManager.h"
#include "cxTool.h"
#include "cxTracker.h"

namespace cx
{
/**
 * \class ToolConfigurationParser
 *
 * \brief Class for reading the configuration file that sets up
 * tracking for CustusX3
 *
 * \date 15. juni 2010
 * \author: jbake
 */
class ToolConfigurationParser
{
public:
  ToolConfigurationParser(std::string& configXmlFilePath);
  ~ToolConfigurationParser();

  void setLoggingFolder(std::string& loggingFolder);
  bool readConfigurationFile();
  TrackerPtr getTracker();
  ssc::ToolManager::ToolMapPtr getConfiguredTools();

private:
  std::string mConfigurationPath; ///< path to the configuration file
  std::string mLoggingFolder;         ///< path to where logging should be saved
  const std::string mTrackerTag, mTrackerTypeTag, mToolfileTag, mToolTag,
                    mToolTypeTag, mToolIdTag, mToolNameTag,
                    mToolGeoFileTag, mToolSensorTag, mToolSensorTypeTag,
                    mToolSensorWirelessTag, mToolSensorDOFTag, mToolSensorPortnumberTag,
                    mToolSensorChannelnumberTag, mToolSensorRomFileTag,
                    mToolCalibrationTag, mToolCalibrationFileTag;
                    ///< names of necessary tags in the configuration file

  QDomDocument mConfigureDoc;
  QList<QDomNodeList> mToolNodeList;
};

} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */
