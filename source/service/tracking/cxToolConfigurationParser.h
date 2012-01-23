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
#include "cxIgstkTool.h"

namespace cx
{

/**
 * \brief Class for reading the files defining a CustusX3 tool
 * \ingroup cxServiceTracking
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
 * \brief Class for reading the files defining a CustusX3 tool
 * \ingroup cxServiceTracking
 *
 * \date 21. march 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ToolFileParser
{
public:
  ToolFileParser(QString absoluteToolFilePath, QString loggingFolder = "");
  ~ToolFileParser();

  IgstkTool::InternalStructure getTool();

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

/**
* @}
*/
} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */
