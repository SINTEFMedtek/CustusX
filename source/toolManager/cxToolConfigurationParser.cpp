#include "cxToolConfigurationParser.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{
//----------------------------------------------------------------------------------------------------------------------
ToolConfigurationParser::ToolConfigurationParser(QString& configXmlFilePath, QString loggingFolder) :
      mLoggingFolder(loggingFolder), mTrackerTag("tracker"),
      mToolfileTag("toolfile"), mToolTag("tool"), mToolTypeTag("type"), mToolIdTag("id"), mToolNameTag("name"),
      mToolGeoFileTag("geo_file"), mToolSensorTag("sensor"),
      mToolSensorTypeTag("type"), mToolSensorWirelessTag("wireless"),
      mToolSensorDOFTag("DOF"), mToolSensorPortnumberTag("portnumber"),
      mToolSensorChannelnumberTag("channelnumber"), mToolSensorReferencePointTag("reference_point"),
      mToolSensorRomFileTag("rom_file"), mToolCalibrationTag("calibration"),
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
  mConfigurationPath = configurationFileInfo.absolutePath()+"/";

  if(mLoggingFolder.isEmpty())
    mLoggingFolder = mConfigurationPath;
}

ToolConfigurationParser::~ToolConfigurationParser()
{}

IgstkTracker::InternalStructure ToolConfigurationParser::getTracker()
{
  QList<QDomNode> trackerNodeList = this->getTrackerNodeList();

  IgstkTracker::InternalStructure internalStructure;
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
        internalStructure.mType = IgstkTracker::TRACKER_POLARIS_SPECTRA;
      } else if (text.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mType = IgstkTracker::TRACKER_POLARIS_VICRA;
      } else
      {
        internalStructure.mType = IgstkTracker::TRACKER_POLARIS;
      }
    } else if (text.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mType = IgstkTracker::TRACKER_AURORA;
    } else if (text.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mType = IgstkTracker::TRACKER_MICRON;
    } else
    {
      internalStructure.mType = IgstkTracker::TRACKER_NONE;
    }
    internalStructure.mLoggingFolderName = this->getLoggingFolder();
  }
  return internalStructure;
}

std::vector<Tool::InternalStructure> ToolConfigurationParser::getConfiguredTools()
{
  std::vector<QString> toolFolderAbsolutePaths;
  QList<QDomNode> toolNodeList = this->getToolNodeList(toolFolderAbsolutePaths);

  std::vector<Tool::InternalStructure> tools;
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
      toolGeofileText = toolFolderAbsolutePaths.at(i) + toolGeofileText;
    internalStructure.mGraphicsFileName = toolGeofileText;    QDomElement toolInstrumentElement = toolNode.firstChildElement(mInstrumentTag);
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
        internalStructure.mTrackerType = IgstkTracker::TRACKER_POLARIS_SPECTRA;
      } else if (toolSensorTypeText.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mTrackerType = IgstkTracker::TRACKER_POLARIS_VICRA;
      } else
      {
        internalStructure.mTrackerType = IgstkTracker::TRACKER_POLARIS;
      }
    } else if (toolSensorTypeText.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = IgstkTracker::TRACKER_AURORA;
    } else if (toolSensorTypeText.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = IgstkTracker::TRACKER_MICRON;
    } else
    {
      internalStructure.mTrackerType = IgstkTracker::TRACKER_NONE;
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

    QDomNodeList toolSensorReferencePointList = toolSensorElement.elementsByTagName(mToolSensorReferencePointTag);
    for (int j = 0; j < toolSensorReferencePointList.count(); j++)
    {
      QDomNode node = toolSensorReferencePointList.item(j);
      if(!node.hasAttributes())
      {
        ssc::messageManager()->sendWarning("Found reference point without id attribute. Skipping.");
        continue;
      }
      bool ok;
      int id = node.toElement().attribute("id").toInt(&ok);
      if(!ok)
      {
        ssc::messageManager()->sendWarning("Attribute id of a reference point was not an int. Skipping.");
        continue;
      }
      QString toolSensorReferencePointText = node.toElement().text();
      ssc::Vector3D vector = ssc::Vector3D::fromString(toolSensorReferencePointText);
      internalStructure.mReferencePoints[id] = vector;
    }

    QDomElement toolSensorRomFileElement = toolSensorElement.firstChildElement(mToolSensorRomFileTag);
    QString toolSensorRomFileText = toolSensorRomFileElement.text();
    if (!toolSensorRomFileText.isEmpty())
      toolSensorRomFileText = toolFolderAbsolutePaths.at(i) + toolSensorRomFileText;
    internalStructure.mSROMFilename = toolSensorRomFileText;

    QDomElement toolCalibrationElement = toolNode.firstChildElement(mToolCalibrationTag);
    if (toolCalibrationElement.isNull())
    {
      ssc::messageManager()->sendError(
          "Could not find the <calibration> tag under the <tool> tag. Aborting this tool.");
      continue;
    }
    QDomElement toolCalibrationFileElement = toolCalibrationElement.firstChildElement(mToolCalibrationFileTag);
    QString toolCalibrationFileText = toolCalibrationFileElement.text();
    if (!toolCalibrationFileText.isEmpty())
      toolCalibrationFileText = toolFolderAbsolutePaths.at(i) + toolCalibrationFileText;
    internalStructure.mCalibrationFilename = toolCalibrationFileText;
    internalStructure.mCalibration = this->readCalibrationFile(internalStructure.mCalibrationFilename);

    internalStructure.mTransformSaveFileName = mLoggingFolder;
    internalStructure.mLoggingFolderName = mLoggingFolder;
    tools.push_back(internalStructure);
  }
  return tools;
}

QString ToolConfigurationParser::getUserManual() const
{
  QString retval;

  return retval;
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
    QString toolFilename = toolFileList.item(i).firstChild().nodeValue();

    //check if path is file or folder
    QString filepath = mConfigurationPath+toolFilename; //absolute path to tool file or folder
    QFileInfo toolFileInfo(filepath);
    if(toolFileInfo.isDir())
    {
      QString name = toolFileInfo.fileName();
      filepath += "/"+name.append(".xml");
    }

    if(toolFilename.isEmpty())
    {
      ssc::messageManager()->sendError("A toolfile tag in the config xml file is not correctly formated. Skipping it.");
      continue;
    }

    QFile toolFile(filepath);
    if (!toolFile.exists())
    {
      ssc::messageManager()->sendError(filepath+" does not exists. Skipping this tool.");
      continue;
    }
    QDomDocument toolDoc;
    if (!toolDoc.setContent(&toolFile))
    {
      ssc::messageManager()->sendError("Could not set the xml content of the tool file "+toolFilename);
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

igstk::Transform ToolConfigurationParser::readCalibrationFile(QString filename)
{
  igstk::Transform retval;

  itk::Matrix<double, 3, 3> calMatrix;
  itk::Versor<double> rotation;
  itk::Vector<double, 3> translation;

  /* File must be in the form
   * rot_00 rot_01 rot_02 trans_0
   * rot_10 rot_11 rot_12 trans_1
   * rot_20 rot_21 rot_22 trans_2
   */
  std::ifstream inputStream;
  inputStream.open(cstring_cast(filename));
  if(inputStream.is_open())
  {
    std::string line;
    int lineNumber = 0;
    while(!inputStream.eof() && lineNumber<3)
    {
      getline(inputStream, line);

      for(int i = 0; i<4; i++)
      {
        //Tolerating more than one blank space between numbers
        while(line.find(" ") == 0)
        {
          line.erase(0,1);
        }
        std::string::size_type pos = line.find(" ");
        std::string str;
        if(pos != std::string::npos)
        {
          str = line.substr(0, pos);
        }
        else
        {
          str = line;
        }
        double d = atof(str.c_str());
        if(i<3)
        {
          calMatrix(lineNumber, i) = d;
        }
        if(i == 3)
        {
          translation.SetElement(lineNumber, d);
        }
        line.erase(0, pos);
      }
      lineNumber++;
    }
    rotation.Set(calMatrix);
    retval.SetTranslationAndRotation(translation, rotation, 1.0, igstk::TimeStamp::GetLongestPossibleTime());
  }
  inputStream.close();
  return retval;
}

//----------------------------------------------------------------------------------------------------------------------

ConfigurationFileParser::ConfigurationFileParser(QString absoluteConfigFilePath) :
    mConfigurationFilePath(absoluteConfigFilePath)
{}

ConfigurationFileParser::~ConfigurationFileParser()
{}

std::vector<IgstkTracker::InternalStructure> ConfigurationFileParser::getTrackers()
{
  std::vector<IgstkTracker::InternalStructure> retval;
  //TODO
  return retval;
}

std::vector<QString> ConfigurationFileParser::getToolFilePaths()
{
  std::vector<QString> retval;
  //TODO
  return retval;
}

QString ConfigurationFileParser::getUserManual() const
{
  QString retval;

  //TODO
//  QString intro = "<p>"
//      "A configuration file is the document CustusX3 uses to set up tracking. It defines what "
//      "tracking system(s) to use and what tools to attach."
//      "</p>";
//
//  QString rules = "<p>"
//      "File names should be constructed using the following pattern: \<tracking-system\>_*\<tool-name\>_*\<optional\>.xml <br>."
//      "Example: POLARIS_AccuracyPhantom_NDIpointer_SWPointer_Ultrasonix_L14-5.xml <br>"
//      "</p>";
//
//  QString tags = "<p>"
//      "The following tags are required: <br>"
//      "<ul>"
//      "<li>\<configuration\>: no value required <\li>"
//      "<li>\<tracker\>: no value required </li>"
//      "<li>\<type\>: valid values = [polaris, polaris spectra, polaris vicra, aurora, micron] </li>"
//      "<li>\<toolfile type=\"\" reference=\"\" \>: valid value = relative path to a valid CustusX3 tool file </li>"
//      "</ul>"
//      "</p>";
//
//  QString example = "<p></p>";
//
//  retval.append("INTRO<br>");
//  retval.append(intro);
//  retval.append("RULES<br>");
//  retval.append(rules);
//  retval.append("TAGS<br>");
//  retval.append(tags);
//  retval.append("EXAMPLE<br>");
//  retval.append(example);

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------

ToolFileParser::ToolFileParser(QString absoluteToolFilePath) :
    mToolFilePath(absoluteToolFilePath)
{}

ToolFileParser::~ToolFileParser()
{}

Tool::InternalStructure ToolFileParser::getTool()
{
  Tool::InternalStructure retval;
  //TODO
  return retval;
}

QString ToolFileParser::getUserManual() const
{
  QString retval;
  //TODO
  return retval;
}

igstk::Transform ToolFileParser::readCalibrationFile(QString absoluteFilePath)
{
  igstk::Transform retval;

  itk::Matrix<double, 3, 3> calMatrix;
  itk::Versor<double> rotation;
  itk::Vector<double, 3> translation;

  /* File must be in the form
   * rot_00 rot_01 rot_02 trans_0
   * rot_10 rot_11 rot_12 trans_1
   * rot_20 rot_21 rot_22 trans_2
   */
  std::ifstream inputStream;
  inputStream.open(cstring_cast(absoluteFilePath));
  if(inputStream.is_open())
  {
    std::string line;
    int lineNumber = 0;
    while(!inputStream.eof() && lineNumber<3)
    {
      getline(inputStream, line);

      for(int i = 0; i<4; i++)
      {
        //Tolerating more than one blank space between numbers
        while(line.find(" ") == 0)
        {
          line.erase(0,1);
        }
        std::string::size_type pos = line.find(" ");
        std::string str;
        if(pos != std::string::npos)
        {
          str = line.substr(0, pos);
        }
        else
        {
          str = line;
        }
        double d = atof(str.c_str());
        if(i<3)
        {
          calMatrix(lineNumber, i) = d;
        }
        if(i == 3)
        {
          translation.SetElement(lineNumber, d);
        }
        line.erase(0, pos);
      }
      lineNumber++;
    }
    rotation.Set(calMatrix);
    retval.SetTranslationAndRotation(translation, rotation, 1.0, igstk::TimeStamp::GetLongestPossibleTime());
  }
  inputStream.close();
  return retval;
}

//----------------------------------------------------------------------------------------------------------------------

} //namespace cx
