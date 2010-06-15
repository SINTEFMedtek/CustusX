#include "cxToolConfigurationParser.h"

//#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "cxMessageManager.h"

namespace cx
{
ToolConfigurationParser::ToolConfigurationParser(std::string& configXmlFilePath) :
      mTrackerTag("tracker"),
      mTrackerTypeTag("type"), mToolfileTag("toolfile"), mToolTag("tool"),
      mToolTypeTag("type"), mToolIdTag("id"), mToolNameTag("name"),
      mToolGeoFileTag("geo_file"), mToolSensorTag("sensor"),
      mToolSensorTypeTag("type"), mToolSensorWirelessTag("wireless"),
      mToolSensorDOFTag("DOF"), mToolSensorPortnumberTag("portnumber"),
      mToolSensorChannelnumberTag("channelnumber"), mToolSensorRomFileTag(
          "rom_file"), mToolCalibrationTag("calibration"),
      mToolCalibrationFileTag("cal_file")
{
  QFile configurationFile(QString(configXmlFilePath.c_str()));
  QFileInfo configurationFileInfo(configurationFile);
  QString configurationPath = configurationFileInfo.path() + "/";

  if (!configurationFile.open(QIODevice::ReadOnly))
  {
    messageManager()->sendInfo("Could not open " + configXmlFilePath + ".");
    return;
  }
  if (!mConfigureDoc.setContent(&configurationFile))
  {
    messageManager()->sendInfo("Could not set the xml content of the file "+ configXmlFilePath);
    return;
  }
  //std::cout << mConfigureDoc.toString().toStdString() << std::endl;
  mConfigurationPath = configurationFileInfo.absolutePath().toStdString()+"/";
  //std::cout << "mConfigurationPath: " << mConfigurationPath << std::endl;
}

ToolConfigurationParser::~ToolConfigurationParser()
{}

void ToolConfigurationParser::setLoggingFolder(std::string& loggingFolder)
{
  mLoggingFolder = loggingFolder;
}

bool ToolConfigurationParser::readConfigurationFile()
{
  //tracker
  QDomNodeList trackerNodeList = mConfigureDoc.elementsByTagName(QString(mTrackerTag.c_str()));

  //tools
  QDomNodeList toolFileList = mConfigureDoc.elementsByTagName(QString(mToolfileTag.c_str()));
  for (int i = 0; i < toolFileList.count(); i++)
  {
    std::string iString = "" + i;
    QDomNode filenameNode = toolFileList.item(i).firstChild();
    if (filenameNode.isNull())
    {
      messageManager()->sendInfo("Toolfiletag " + iString+ " does not containe any usefull info. Skipping this tool.");
      continue;
    }
    QString filename = filenameNode.nodeValue();
    if (filename.isEmpty())
    {
      messageManager()->sendInfo("Toolfiletag " + iString+ " does not contain readable text. Skipping this tool.");
      continue;
    }
    //QFile toolFile(configurationPath + filename);
    std::string filepath = mConfigurationPath+"/"+filename.toStdString();
    QFile toolFile(QString(filepath.c_str()));
    QDir dir;
    if (!toolFile.exists())
    {
      messageManager()->sendInfo(filepath+ " does not exists. Skipping this tool.");
      continue;
    } else
    {
      messageManager()->sendInfo(filename.toStdString() + " exists.");
    }
    QDomDocument toolDoc;
    if (!toolDoc.setContent(&toolFile))
    {
      messageManager()->sendInfo("Could not set the xml content of the file "
          + filename.toStdString());
      continue;
    }
    QDomNodeList toolList = toolDoc.elementsByTagName(QString(mToolTag.c_str()));
    mToolNodeList.push_back(toolList);
  }
  return true;
}

TrackerPtr ToolConfigurationParser::getTracker()
{
  QDomNodeList trackerNodeList = mConfigureDoc.elementsByTagName(QString(mTrackerTag.c_str()));
  std::vector<TrackerPtr> trackers;
  Tracker::InternalStructure internalStructure;
  for (int i = 0; i < trackerNodeList.count(); i++)
  {
    std::string iString = "" + i;
    QDomNode trackerNode = trackerNodeList.at(i);
    const QDomElement trackerType = trackerNode.firstChildElement(QString(mTrackerTypeTag.c_str()));
    if (trackerType.isNull())
    {
      messageManager()->sendInfo("Tracker " + iString + " does not have the required tag <type>.");
      continue;
    }
    QString text = trackerType.text();
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
    internalStructure.mLoggingFolderName = mLoggingFolder;
    trackers.push_back(TrackerPtr(new Tracker(internalStructure)));
  }
  if (trackers.empty())
  {
    internalStructure.mType = Tracker::TRACKER_NONE;
    internalStructure.mLoggingFolderName = mLoggingFolder;
    trackers.push_back(TrackerPtr(new Tracker(internalStructure)));
  }
  return trackers.at(0);
}

ssc::ToolManager::ToolMapPtr ToolConfigurationParser::getConfiguredTools()
{
  QFile configurationFile(QString(mConfigurationPath.c_str()));
  QFileInfo configurationFileInfo(configurationFile);
  //QString configurationPath = configurationFileInfo.path() + "/";

  ssc::ToolManager::ToolMapPtr tools(new ssc::ToolManager::ToolMap());
  QDomNode node;
  for (int i = 0; i < mToolNodeList.size(); i++)
  {
    Tool::InternalStructure internalStructure;
    QDomNodeList toolNodes = mToolNodeList.at(i);
    if (toolNodes.size() < 1)
    {
      messageManager()->sendInfo("Found no <tool> tags in the toolxmlfile.");
      continue;
    }
    QDomNode toolNode = toolNodes.item(0); //A toolfile should only contain 1 tool tag
    if (toolNode.isNull())
    {
      messageManager()->sendInfo("Could not read the <tool> tag.");
      continue;
    }

    QDomElement toolTypeElement = toolNode.firstChildElement(QString(mToolTypeTag.c_str()));
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

    QDomElement toolIdElement = toolNode.firstChildElement(QString(mToolIdTag.c_str()));
    QString toolIdText = toolIdElement.text();
    internalStructure.mUid = toolIdText.toStdString();

    QDomElement toolNameElement = toolNode.firstChildElement(QString(mToolNameTag.c_str()));
    QString toolNameText = toolNameElement.text();
    internalStructure.mName = toolNameText.toStdString();

    QDomElement toolGeofileElement = toolNode.firstChildElement(QString(mToolGeoFileTag.c_str()));
    QString toolGeofileText = toolGeofileElement.text();
    if (!toolGeofileText.isEmpty())
      toolGeofileText = QString(mConfigurationPath.c_str()) + toolGeofileText;
    internalStructure.mGraphicsFileName = toolGeofileText.toStdString();

    QDomElement toolSensorElement = toolNode.firstChildElement(QString(mToolSensorTag.c_str()));
    if (toolSensorElement.isNull())
    {
      messageManager()->sendInfo(
          "Could not find the <sensor> tag under the <tool> tag. Aborting tihs tool.");
      continue;
    }
    QDomElement toolSensorTypeElement = toolSensorElement.firstChildElement(QString(mToolSensorTypeTag.c_str()));
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
        toolSensorElement.firstChildElement(QString(mToolSensorWirelessTag.c_str()));
    QString toolSensorWirelessText = toolSensorWirelessElement.text();
    if (toolSensorWirelessText.contains("yes", Qt::CaseInsensitive))
      internalStructure.mWireless = true;
    else if (toolSensorWirelessText.contains("no", Qt::CaseInsensitive))
      internalStructure.mWireless = false;

    QDomElement toolSensorDOFElement = toolSensorElement.firstChildElement(QString(mToolSensorDOFTag.c_str()));
    QString toolSensorDOFText = toolSensorDOFElement.text();
    if (toolSensorDOFText.contains("5", Qt::CaseInsensitive))
      internalStructure.m5DOF = true;
    else if (toolSensorDOFText.contains("6", Qt::CaseInsensitive))
      internalStructure.m5DOF = false;

    QDomElement toolSensorPortnumberElement =
        toolSensorElement.firstChildElement(QString(mToolSensorPortnumberTag.c_str()));
    QString toolSensorPortnumberText = toolSensorPortnumberElement.text();
    internalStructure.mPortNumber = toolSensorPortnumberText.toInt();

    QDomElement toolSensorChannelnumberElement =
        toolSensorElement.firstChildElement(QString(mToolSensorChannelnumberTag.c_str()));
    QString toolSensorChannelnumberText = toolSensorChannelnumberElement.text();
    internalStructure.mChannelNumber = toolSensorChannelnumberText.toInt();

    QDomElement toolSensorRomFileElement = toolSensorElement.firstChildElement(QString(mToolSensorRomFileTag.c_str()));
    QString toolSensorRomFileText = toolSensorRomFileElement.text();
    if (!toolSensorRomFileText.isEmpty())
      toolSensorRomFileText = QString(mConfigurationPath.c_str()) + toolSensorRomFileText;
    internalStructure.mSROMFilename = toolSensorRomFileText.toStdString();

    QDomElement toolCalibrationElement = toolNode.firstChildElement(QString(mToolCalibrationTag.c_str()));
    if (toolCalibrationElement.isNull())
    {
      messageManager()->sendInfo(
          "Could not find the <calibration> tag under the <tool> tag. Aborting this tool.");
      continue;
    }
    QDomElement toolCalibrationFileElement =
        toolCalibrationElement.firstChildElement(QString(mToolCalibrationFileTag.c_str()));
    QString toolCalibrationFileText = toolCalibrationFileElement.text();
    if (!toolCalibrationFileText.isEmpty())
      toolCalibrationFileText = QString(mConfigurationPath.c_str()) + toolCalibrationFileText;
    internalStructure.mCalibrationFilename = toolCalibrationFileText.toStdString();

    internalStructure.mTransformSaveFileName = mLoggingFolder;
    internalStructure.mLoggingFolderName = mLoggingFolder;

    Tool* cxTool = new Tool(internalStructure);
    ssc::ToolPtr tool(cxTool);
    (*tools)[tool->getUid()] = tool;
    messageManager()->sendInfo("Done configuring a tool with uid: "+tool->getUid());
  }
  return tools;
}

} //namespace cx
