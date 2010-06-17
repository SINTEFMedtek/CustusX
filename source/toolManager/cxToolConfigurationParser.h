#ifndef CXTOOLCONFIGURATIONPARSER_H_
#define CXTOOLCONFIGURATIONPARSER_H_

#include <vector>
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
 * \warning Only supports configurations with one tracker at the moment
 *
 * \date 15. juni 2010
 * \author: Janne Beate Bakeng
 */
class ToolConfigurationParser
{
public:
  ToolConfigurationParser(std::string& configXmlFilePath, std::string loggingFolder = ""); ///< opens the xml file and readies it for reading
  ~ToolConfigurationParser();

  TrackerPtr getTracker(); ///< return the tracker created from the xml file
  ssc::ToolManager::ToolMapPtr getConfiguredTools(); ///< return all tools created from the xml files

private:
  std::string     getLoggingFolder() const;
  QList<QDomNode> getTrackerNodeList(); ///< returns a list of tracker nodes
  QList<QDomNode> getToolNodeList(std::vector<QString>& toolFolderAbsolutePaths); ///< returns a list of tool nodes

  std::string mConfigurationPath; ///< path to the configuration file
  std::string mLoggingFolder; ///< path to where logging should be saved, default is the folder where configfile is found
  const QString mTrackerTag, mTrackerTypeTag, mToolfileTag, mToolTag,
                    mToolTypeTag, mToolIdTag, mToolNameTag,
                    mToolGeoFileTag, mToolSensorTag, mToolSensorTypeTag,
                    mToolSensorWirelessTag, mToolSensorDOFTag, mToolSensorPortnumberTag,
                    mToolSensorChannelnumberTag, mToolSensorRomFileTag,
                    mToolCalibrationTag, mToolCalibrationFileTag;
                    ///< names of necessary tags in the configuration file

  QDomDocument mConfigureDoc; ///< the config xml document
};

} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */
