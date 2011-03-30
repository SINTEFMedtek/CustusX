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
                    mToolTypeTag, mToolIdTag, mToolNameTag, mToolClinicalAppTag,
                    mToolGeoFileTag, mToolSensorTag, mToolSensorTypeTag,
                    mToolSensorWirelessTag, mToolSensorDOFTag, mToolSensorPortnumberTag,
                    mToolSensorChannelnumberTag, mToolSensorReferencePointTag ,mToolSensorRomFileTag,
                    mToolCalibrationTag, mToolCalibrationFileTag,
                    mInstrumentTag, mInstrumentIdTag, mInstrumentScannerIdTag;
                    ///< names of necessary tags in the configuration setup

  QDomDocument mConfigureDoc; ///< the config xml document

  igstk::Transform readCalibrationFile(QString filename);

};

/**
 * \class ConfigurationFileParser
 *
 * \brief Class for reading the files defining a CustusX3 tool
 *
 * \date 21. march 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ConfigurationFileParser
{
public:
  ConfigurationFileParser(QString absoluteConfigFilePath);
  ~ConfigurationFileParser();

  std::vector<ssc::MEDICAL_DOMAIN> getApplicationDomains();
  std::vector<IgstkTracker::InternalStructure> getTrackers();
  std::vector<QString> getAbsoluteToolFilePaths();
  QString getAbsoluteReferenceFilePath();

 static QString getTemplatesAbsoluteFilePath();
 static void saveConfiguration();

private:
   void setConfigDocument(QString configAbsoluteFilePath);
   bool isConfigFileValid();
   QString getAbsoluteToolFilePath(QDomElement toolfileelement);

  QString mConfigurationFilePath; ///< absolute path to the configuration file
  QDomDocument mConfigureDoc; ///< the config xml document
  const QString mConfigTag, mConfigTrackerTag, mConfigTrackerToolFile; ///< names of necessary tags in the configuration file
};

/**
 * \class ToolFileParser
 *
 * \brief Class for reading the files defining a CustusX3 tool
 *
 * \date 21. march 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ToolFileParser
{
public:
  ToolFileParser(QString absoluteToolFilePath);
  ~ToolFileParser();

  Tool::InternalStructure getTool();

  static QString getTemplatesAbsoluteFilePath();

private:
  QDomNode getToolNode(QString toolAbsoluteFilePath);
  igstk::Transform readCalibrationFile(QString absoluteFilePath);

  QString mToolFilePath; ///< absolutepath to the tool file
  QDomDocument mToolDoc; ///< the tool xml document
  const QString mToolfileTag, mToolTag, mToolTypeTag, mToolIdTag, mToolNameTag, mToolClinicalAppTag,
                    mToolGeoFileTag, mToolSensorTag, mToolSensorTypeTag,
                    mToolSensorWirelessTag, mToolSensorDOFTag, mToolSensorPortnumberTag,
                    mToolSensorChannelnumberTag, mToolSensorReferencePointTag ,mToolSensorRomFileTag,
                    mToolCalibrationTag, mToolCalibrationFileTag,
                    mInstrumentTag, mInstrumentIdTag, mInstrumentScannerIdTag;
                    ///< names of necessary tags in the tool file
};

} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */
