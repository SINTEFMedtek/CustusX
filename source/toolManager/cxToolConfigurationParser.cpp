#include "cxToolConfigurationParser.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "sscMessageManager.h"

namespace cx
{
ToolConfigurationParser::ToolConfigurationParser(QString& configXmlFilePath, QString loggingFolder) :
      mLoggingFolder(loggingFolder), mTrackerTag("tracker"),
      mTrackerTypeTag("type"), mToolfileTag("toolfile"), mToolTag("tool"),
      mToolTypeTag("type"), mToolIdTag("id"), mToolNameTag("name"),
      mToolGeoFileTag("geo_file"), mToolSensorTag("sensor"),
      mToolSensorTypeTag("type"), mToolSensorWirelessTag("wireless"),
      mToolSensorDOFTag("DOF"), mToolSensorPortnumberTag("portnumber"),
      mToolSensorChannelnumberTag("channelnumber"), mToolSensorRomFileTag(
          "rom_file"), mToolCalibrationTag("calibration"),
      mToolCalibrationFileTag("cal_file"),
      mInstrumentTag("instrument"), mInstrumentIdTag("id"), mInstrumentScannerIdTag("scannerid")
{
  QFile configurationFile(configXmlFilePath);
  QFileInfo configurationFileInfo(configurationFile);
  QString configurationPath = configurationFileInfo.path() + "/";

  if (!configurationFile.open(QIODevice::ReadOnly))
  {
    ssc::messageManager()->sendError("Could not open " + configXmlFilePath + ".");
    return;
  }
  if (!mConfigureDoc.setContent(&configurationFile))
  {
    ssc::messageManager()->sendError("Could not set the xml content of the file "+ configXmlFilePath);
    return;
  }
  //std::cout << mConfigureDoc.toString().toStdString() << std::endl;
  mConfigurationPath = configurationFileInfo.absolutePath()+"/";
  //std::cout << "mConfigurationPath: " << mConfigurationPath << std::endl;

  if(mLoggingFolder.isEmpty())
    mLoggingFolder = mConfigurationPath;
}

ToolConfigurationParser::~ToolConfigurationParser()
{}

TrackerPtr ToolConfigurationParser::getTracker()
{
  QList<QDomNode> trackerNodeList = this->getTrackerNodeList();

  std::vector<TrackerPtr> trackers;
  Tracker::InternalStructure internalStructure;
  for (int i = 0; i < trackerNodeList.count(); i++)
  {
    QString iString = "" + i;
    QDomNode trackerNode = trackerNodeList.at(i);
    const QDomElement trackerTypeElement = trackerNode.firstChildElement(mTrackerTypeTag);
    if (trackerTypeElement.isNull())
    {
      ssc::messageManager()->sendError("Tracker " + iString + " does not have the required tag <type>.");
      continue;
    }
    QString text = trackerTypeElement.text();
    if (text.contains("polaris", Qt::CaseInsensitive))
    {
      if (text.contains("spectra", Qt::CaseInsensitive))
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS_SPECTRA;
      } else if (text.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS_VICRA;
      } else
      {
        internalStructure.mType = Tracker::TRACKER_POLARIS;
      }
    } else if (text.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mType = Tracker::TRACKER_AURORA;
    } else if (text.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mType = Tracker::TRACKER_MICRON;
    } else
    {
      internalStructure.mType = Tracker::TRACKER_NONE;
    }
    internalStructure.mLoggingFolderName = this->getLoggingFolder();

    TrackerPtr tracker(new Tracker(internalStructure));
    if(tracker->isValid())
      trackers.push_back(tracker);
  }

  if (trackers.empty())
  {
    internalStructure.mType = Tracker::TRACKER_NONE;
    internalStructure.mLoggingFolderName = this->getLoggingFolder();
    trackers.push_back(TrackerPtr(new Tracker(internalStructure)));
  }

  return trackers.at(0);
}

ssc::ToolManager::ToolMapPtr ToolConfigurationParser::getConfiguredTools()
{
  std::vector<QString> toolFolderAbsolutePaths;
  QList<QDomNode> toolNodeList = this->getToolNodeList(toolFolderAbsolutePaths);

  ssc::ToolManager::ToolMapPtr tools(new ssc::ToolManager::ToolMap());
  //QDomNode node;
  for (int i = 0; i < toolNodeList.size(); i++)
  {
    Tool::InternalStructure internalStructure;

    QDomNode toolNode = toolNodeList.at(i);
    if (toolNode.isNull())
    {
      ssc::messageManager()->sendError("Could not read the <tool> tag.");
      continue;
    }

    QDomElement toolTypeElement = toolNode.firstChildElement(mToolTypeTag);
    QString toolTypeText = toolTypeElement.text();
    if (toolTypeText.contains("reference", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_REFERENCE;
    } else if (toolTypeText.contains("pointer", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_POINTER;
    } else if (toolTypeText.contains("usprobe", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::Tool::TOOL_US_PROBE;
    } else
    {
      internalStructure.mType = ssc::Tool::TOOL_NONE;
    }

    QDomElement toolIdElement = toolNode.firstChildElement(mToolIdTag);
    QString toolIdText = toolIdElement.text();
    internalStructure.mUid = toolIdText;

    QDomElement toolNameElement = toolNode.firstChildElement(mToolNameTag);
    QString toolNameText = toolNameElement.text();
    internalStructure.mName = toolNameText;

    QDomElement toolGeofileElement = toolNode.firstChildElement(mToolGeoFileTag);
    QString toolGeofileText = toolGeofileElement.text();
    if (!toolGeofileText.isEmpty())
      //toolGeofileText = QString(mConfigurationPath.c_str()) + toolGeofileText;
      toolGeofileText = toolFolderAbsolutePaths.at(i) + toolGeofileText;
    internalStructure.mGraphicsFileName = toolGeofileText;

    QDomElement toolInstrumentElement = toolNode.firstChildElement(mInstrumentTag);
    if (toolInstrumentElement.isNull())
    {
      ssc::messageManager()->sendError("Could not find the <instrument> tag under the <tool> tag. Aborting this tool.");
      continue;
    }
    QDomElement toolInstrumentIdElement =
        toolInstrumentElement.firstChildElement(mInstrumentIdTag);
    QString toolInstrumentIdText = toolInstrumentIdElement.text();
    internalStructure.mInstrumentId = toolInstrumentIdText;

    QDomElement toolInstrumentScannerIdElement =
        toolInstrumentElement.firstChildElement(mInstrumentScannerIdTag);
    QString toolInstrumentScannerIdText = toolInstrumentScannerIdElement.text();
    internalStructure.mInstrumentScannerId = toolInstrumentScannerIdText;

    QDomElement toolSensorElement = toolNode.firstChildElement(mToolSensorTag);
    if (toolSensorElement.isNull())
    {
      ssc::messageManager()->sendError("Could not find the <sensor> tag under the <tool> tag. Aborting this tool.");
      continue;
    }
    QDomElement toolSensorTypeElement = toolSensorElement.firstChildElement(mToolSensorTypeTag);
    QString toolSensorTypeText = toolSensorTypeElement.text();
    if (toolSensorTypeText.contains("polaris", Qt::CaseInsensitive))
    {
      if (toolSensorTypeText.contains("spectra", Qt::CaseInsensitive))
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS_SPECTRA;
      } else if (toolSensorTypeText.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS_VICRA;
      } else
      {
        internalStructure.mTrackerType = Tracker::TRACKER_POLARIS;
      }
    } else if (toolSensorTypeText.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = Tracker::TRACKER_AURORA;
    } else if (toolSensorTypeText.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = Tracker::TRACKER_MICRON;
    } else
    {
      internalStructure.mTrackerType = Tracker::TRACKER_NONE;
    }

    QDomElement toolSensorWirelessElement =
        toolSensorElement.firstChildElement(mToolSensorWirelessTag);
    QString toolSensorWirelessText = toolSensorWirelessElement.text();
    if (toolSensorWirelessText.contains("yes", Qt::CaseInsensitive))
      internalStructure.mWireless = true;
    else if (toolSensorWirelessText.contains("no", Qt::CaseInsensitive))
      internalStructure.mWireless = false;

    QDomElement toolSensorDOFElement = toolSensorElement.firstChildElement(mToolSensorDOFTag);
    QString toolSensorDOFText = toolSensorDOFElement.text();
    if (toolSensorDOFText.contains("5", Qt::CaseInsensitive))
      internalStructure.m5DOF = true;
    else if (toolSensorDOFText.contains("6", Qt::CaseInsensitive))
      internalStructure.m5DOF = false;

    QDomElement toolSensorPortnumberElement =
        toolSensorElement.firstChildElement(mToolSensorPortnumberTag);
    QString toolSensorPortnumberText = toolSensorPortnumberElement.text();
    internalStructure.mPortNumber = toolSensorPortnumberText.toInt();

    QDomElement toolSensorChannelnumberElement =
        toolSensorElement.firstChildElement(mToolSensorChannelnumberTag);
    QString toolSensorChannelnumberText = toolSensorChannelnumberElement.text();
    internalStructure.mChannelNumber = toolSensorChannelnumberText.toInt();

    QDomElement toolSensorRomFileElement = toolSensorElement.firstChildElement(mToolSensorRomFileTag);
    QString toolSensorRomFileText = toolSensorRomFileElement.text();
    if (!toolSensorRomFileText.isEmpty())
      //toolSensorRomFileText = QString(mConfigurationPath.c_str()) + toolSensorRomFileText;
      toolSensorRomFileText = toolFolderAbsolutePaths.at(i) + toolSensorRomFileText;
    internalStructure.mSROMFilename = toolSensorRomFileText;

    QDomElement toolCalibrationElement = toolNode.firstChildElement(mToolCalibrationTag);
    if (toolCalibrationElement.isNull())
    {
      ssc::messageManager()->sendError(
          "Could not find the <calibration> tag under the <tool> tag. Aborting this tool.");
      continue;
    }
    QDomElement toolCalibrationFileElement =
        toolCalibrationElement.firstChildElement(mToolCalibrationFileTag);
    QString toolCalibrationFileText = toolCalibrationFileElement.text();
    if (!toolCalibrationFileText.isEmpty())
      //toolCalibrationFileText = QString(mConfigurationPath.c_str()) + toolCalibrationFileText;
      toolCalibrationFileText = toolFolderAbsolutePaths.at(i) + toolCalibrationFileText;
    internalStructure.mCalibrationFilename = toolCalibrationFileText;

    internalStructure.mTransformSaveFileName = mLoggingFolder;
    internalStructure.mLoggingFolderName = mLoggingFolder;

    Tool* cxTool = new Tool(internalStructure);
    if(cxTool->isValid())
    {
      ssc::ToolPtr tool(cxTool);
      (*tools)[tool->getUid()] = tool;
    }
  }
  return tools;
}

QString ToolConfigurationParser::getLoggingFolder() const
{
  return mLoggingFolder;
}

QList<QDomNode> ToolConfigurationParser::getTrackerNodeList()
{
  //only support one tracker at the moment
  int nrOfSupportedTrackers = 1;

  QList<QDomNode> trackerNodeList;
  for(int i = 0; i<nrOfSupportedTrackers; ++i )
    trackerNodeList.push_back(mConfigureDoc.elementsByTagName(mTrackerTag).item(i));
  return trackerNodeList;
}

/**
 * @param toolFolderAbsolutePaths[out] send in a vector to get the tool.xml files absolute folder path
 * @return the tool node
 */
QList<QDomNode> ToolConfigurationParser::getToolNodeList(std::vector<QString>& toolFolderAbsolutePaths)
{
  QList<QDomNode> toolNodeList;

  QDomNodeList toolFileList = mConfigureDoc.elementsByTagName(mToolfileTag);
  for (int i = 0; i < toolFileList.count(); i++)
  {
    const QString toolFilename = toolFileList.item(i).firstChild().nodeValue();
    if(toolFilename.isEmpty())
    {
      ssc::messageManager()->sendError("A toolfile tag in the config xml file is not correctly formated. Skipping it.");
      continue;
    }

    QDir dir(mConfigurationPath);

    const QString filepath = dir.absoluteFilePath(toolFilename);
    QFile toolFile(filepath);
    if (!toolFile.exists())
    {
      ssc::messageManager()->sendError(filepath+" does not exists. Skipping this tool.");
      continue;
    }
    QDomDocument toolDoc;
    if (!toolDoc.setContent(&toolFile))
    {
      ssc::messageManager()->sendError("Could not set the xml content of the file "+toolFilename);
      continue;
    }
    //there can only be one tool defined in every tool.xml-file, that's why we say ...item(0)
    QDomNode toolNode = toolDoc.elementsByTagName(mToolTag).item(0);
    toolNodeList.push_back(toolNode);
    QString toolFilesAbsoluteFolderPath = QFileInfo(toolFile).absolutePath()+QString("/");
    toolFolderAbsolutePaths.push_back(toolFilesAbsoluteFolderPath);
  }
  return toolNodeList;
}

} //namespace cx
