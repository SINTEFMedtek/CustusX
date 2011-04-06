#ifndef CXTOOLCONFIGURATIONPARSER_H_
#define CXTOOLCONFIGURATIONPARSER_H_

#include <utility>
#include <map>
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
/*class ToolConfigurationParser
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

};*/

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
  typedef std::pair<QString, bool> ToolFileAndReference;
  typedef std::vector<ToolFileAndReference> ToolFilesAndReferenceVector;
  typedef std::map<ssc::TRACKING_SYSTEM,  ToolFilesAndReferenceVector> TrackersAndToolsMap;
  struct Configuration
  {
    QString             mFileName; ///< absolute path and filename for the new config file
    ssc::CLINICAL_APPLICATION mClinical_app; ///< the clinical application this config is made for
    TrackersAndToolsMap mTrackersAndTools; ///< the trackers and tools (relative path) that should be used in the config
    Configuration() :
      mClinical_app(ssc::mdCOUNT)
    {};
  };

public:
  ConfigurationFileParser(QString absoluteConfigFilePath, QString loggingFolder = "");
  ~ConfigurationFileParser();

  ssc::CLINICAL_APPLICATION getApplicationapplication();
  std::vector<IgstkTracker::InternalStructure> getTrackers();
  std::vector<QString> getAbsoluteToolFilePaths();
  QString getAbsoluteReferenceFilePath();

 static QString getTemplatesAbsoluteFilePath();
 static void saveConfiguration(Configuration& config);

private:
   void setConfigDocument(QString configAbsoluteFilePath);
   bool isConfigFileValid();
   QString getAbsoluteToolFilePath(QDomElement toolfileelement); ///< get the absolute toolfile path for a toolfile element containg a relative toolfile path

  QString mConfigurationFilePath; ///< absolute path to the configuration file
  QString mLoggingFolder; ///< absolutepath to the logging folder

  QDomDocument mConfigureDoc; ///< the config xml document
  const QString mConfigTag, mConfigTrackerTag, mConfigTrackerToolFile; ///< names of necessary tags in the configuration file
  const QString mTypeAttribute, mClinicalAppAttribute, mReferenceAttribute; ///< names of necessary attributes in the configuration file
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
  ToolFileParser(QString absoluteToolFilePath, QString loggingFolder = "");
  ~ToolFileParser();

  Tool::InternalStructure getTool();

  static QString getTemplatesAbsoluteFilePath();

private:
  QDomNode getToolNode(QString toolAbsoluteFilePath);
  igstk::Transform readCalibrationFile(QString absoluteFilePath);

  QString mToolFilePath; ///< absolutepath to the tool file
  QString mLoggingFolder; ///< absolutepath to the logging folder

  QDomDocument mToolDoc; ///< the tool xml document
  const QString mToolTag, mToolTypeTag, mToolIdTag, mToolNameTag, mToolDescriptionTag, mToolManufacturerTag,
                    mToolClinicalAppTag, mToolGeoFileTag, mToolPicFileTag, mToolDocFileTag,
                    mToolInstrumentTag, mToolInstrumentTypeTag, mToolInstrumentIdTag, mToolInstrumentNameTag,
                    mToolInstrumentManufacturerTag, mToolInstrumentScannerIdTag, mToolInstrumentDescriptionTag,
                    mToolSensorTag, mToolSensorTypeTag, mToolSensorIdTag, mToolSensorNameTag,
                    mToolSensorWirelessTag, mToolSensorDOFTag, mToolSensorPortnumberTag,
                    mToolSensorChannelnumberTag, mToolSensorReferencePointTag, mToolSensorManufacturerTag,
                    mToolSensorDescriptionTag, mToolSensorRomFileTag, mToolCalibrationTag, mToolCalibrationFileTag;
                    ///< names of necessary tags in the tool file
};

} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */
