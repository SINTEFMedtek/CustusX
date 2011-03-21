#ifndef CXTOOLCONFIGURATIONPARSER_H_
#define CXTOOLCONFIGURATIONPARSER_H_

#include <vector>
#include <QString>
#include <QDomDocument>
#include "sscToolManager.h"
#include "cxTool.h"
#include "cxIgstkTracker.h"

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
  ToolConfigurationParser(QString& configXmlFilePath, QString loggingFolder = ""); ///< opens the xml file and readies it for reading
  ~ToolConfigurationParser();

  IgstkTracker::InternalStructure getTracker(); ///< return the tracker created from the xml file
  std::vector<IgstkTracker::InternalStructure> getTrackers(); ///< return all trackers created from the config xml file
  std::vector<Tool::InternalStructure> getConfiguredTools(); ///< return all tools created from the config xml file

  QString getUserManual() const; ///< documenting how to correctly write a tool configuration file setup

private:
  QString     getLoggingFolder() const;
  QList<QDomNode> getTrackerNodeList(); ///< returns a list of tracker nodes
  QList<QDomNode> getToolNodeList(std::vector<QString>& toolFolderAbsolutePaths); ///< returns a list of tool nodes

  QString mConfigurationPath; ///< path to the configuration file
  QString mLoggingFolder; ///< path to where logging should be saved, default is the folder where configfile is found

  const QString mTrackerTag, mTrackerTypeTag, mToolfileTag, mToolTag,
                    mToolTypeTag, mToolIdTag, mToolNameTag,
                    mToolGeoFileTag, mToolSensorTag, mToolSensorTypeTag,
                    mToolSensorWirelessTag, mToolSensorDOFTag, mToolSensorPortnumberTag,
                    mToolSensorChannelnumberTag, mToolSensorReferencePointTag ,mToolSensorRomFileTag,
                    mToolCalibrationTag, mToolCalibrationFileTag,
                    mInstrumentTag, mInstrumentIdTag, mInstrumentScannerIdTag;
                    ///< names of necessary tags in the configuration setup

  QDomDocument mConfigureDoc; ///< the config xml document

  igstk::Transform readCalibrationFile(QString filename);

};


} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */
