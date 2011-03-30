#include "cxToolConfigurationParser.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscEnumConverter.h"
#include "cxDataLocations.h"

namespace cx
{
//----------------------------------------------------------------------------------------------------------------------
ToolConfigurationParser::ToolConfigurationParser(QString& configXmlFilePath, QString loggingFolder) :
      mLoggingFolder(loggingFolder), mTrackerTag("tracker"),
      mToolfileTag("toolfile"), mToolTag("tool"), mToolTypeTag("type"), mToolIdTag("id"), mToolNameTag("name"),
      mToolClinicalAppTag("clinical_app"),mToolGeoFileTag("geo_file"), mToolSensorTag("sensor"),
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
        internalStructure.mType = ssc::tsPOLARIS_SPECTRA;
      } else if (text.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mType = ssc::tsPOLARIS_VICRA;
      } else
      {
        internalStructure.mType = ssc::tsPOLARIS;
      }
    } else if (text.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::tsAURORA;
    } else if (text.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mType = ssc::tsMICRON;
    } else
    {
      internalStructure.mType = ssc::tsNONE;
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

    QDomElement toolClinicalAppElement = toolNode.firstChildElement(mToolClinicalAppTag);
    QString toolClinicalAppText = toolClinicalAppElement.text();
    QStringList applicationList = toolClinicalAppText.split(" ");
    foreach(QString string, applicationList)
    {
      if(string.isEmpty())
        continue;
      string = string.toLower();
      ssc::MEDICAL_DOMAIN domain = string2enum<ssc::MEDICAL_DOMAIN>(string);
      if(domain != ssc::mdCOUNT)
        internalStructure.mMedicalDomains.push_back(domain);
      else
        ssc::messageManager()->sendWarning("Did not understand the tag <clinical_app>, "+string+" is invalid.");
    }

    QDomElement toolGeofileElement = toolNode.firstChildElement(mToolGeoFileTag);
    QString toolGeofileText = toolGeofileElement.text();
    if (!toolGeofileText.isEmpty())
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
        internalStructure.mTrackerType = ssc::tsPOLARIS_SPECTRA;
      } else if (toolSensorTypeText.contains("vicra", Qt::CaseInsensitive))
      {
        internalStructure.mTrackerType = ssc::tsPOLARIS_VICRA;
      } else
      {
        internalStructure.mTrackerType = ssc::tsPOLARIS;
      }
    } else if (toolSensorTypeText.contains("aurora", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = ssc::tsAURORA;
    } else if (toolSensorTypeText.contains("micron", Qt::CaseInsensitive))
    {
      internalStructure.mTrackerType = ssc::tsMICRON;
    } else
    {
      internalStructure.mTrackerType = ssc::tsNONE;
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
    mConfigurationFilePath(absoluteConfigFilePath),
    mConfigTag("configuration"), mConfigTrackerTag("tracker"), mConfigTrackerToolFile("toolfile")
{
  this->setConfigDocument(mConfigurationFilePath);
}

ConfigurationFileParser::~ConfigurationFileParser()
{}

ssc::MEDICAL_DOMAIN ConfigurationFileParser::getApplicationDomain()
{
  ssc::MEDICAL_DOMAIN retval;

  if(!this->isConfigFileValid())
    return retval;

  QDomNode configNode = mConfigureDoc.elementsByTagName(mConfigTag).at(0);
  QString applicationDomain = configNode.toElement().attribute("clinical_app");
  retval = string2enum<ssc::MEDICAL_DOMAIN>(applicationDomain);
//  std::cout << "In configfile " << mConfigurationFilePath << " found clinical application " << enum2string(retval) << std::endl;

  return retval;
}

std::vector<IgstkTracker::InternalStructure> ConfigurationFileParser::getTrackers()
{
  std::vector<IgstkTracker::InternalStructure> retval;

  if(!this->isConfigFileValid())
    return retval;

  QDomNodeList trackerNodes = mConfigureDoc.elementsByTagName(mConfigTrackerTag);
  for(int i=0; i < trackerNodes.count(); ++i)
  {
    IgstkTracker::InternalStructure internalStructure;
    QString trackerType = trackerNodes.at(i).toElement().attribute("type");
    internalStructure.mType = string2enum<ssc::TRACKING_SYSTEM>(trackerType);
    internalStructure.mLoggingFolderName = ""; //TODO

//    std::cout << "In configfile " << mConfigurationFilePath << " found tracker type " << enum2string(internalStructure.mType) << std::endl;
    retval.push_back(internalStructure);
  }
  return retval;
}

std::vector<QString> ConfigurationFileParser::getAbsoluteToolFilePaths()
{
  std::vector<QString> retval;

  if(!this->isConfigFileValid())
    return retval;

  QDomNodeList toolFileNodes = mConfigureDoc.elementsByTagName(mConfigTrackerToolFile);
  for(int i=0; i < toolFileNodes.count(); ++i)
  {
    QString absoluteToolFilePath = this->getAbsoluteToolFilePath(toolFileNodes.at(i).toElement());
    if(absoluteToolFilePath.isEmpty())
      continue;

    retval.push_back(absoluteToolFilePath);
  }

  return retval;
}

QString ConfigurationFileParser::getAbsoluteReferenceFilePath()
{
  QString retval;

  if(!this->isConfigFileValid())
    return retval;

//  QFile configFile(mConfigurationFilePath);
//  QString configFolderAbsolutePath = QFileInfo(configFile).dir().absolutePath()+"/";
//  std::cout << "configFolderAbsolutePath " << configFolderAbsolutePath << std::endl;

  QDomNodeList toolFileNodes = mConfigureDoc.elementsByTagName(mConfigTrackerToolFile);
  for(int i=0; i < toolFileNodes.count(); ++i)
  {
    QString reference = toolFileNodes.at(i).toElement().attribute("reference");
    if(reference.contains("yes", Qt::CaseInsensitive))
    {
//      std::cout << "Found yes..." << std::endl;
      retval = this->getAbsoluteToolFilePath(toolFileNodes.at(i).toElement());
    }
  }
  return retval;
}

QString ConfigurationFileParser::getTemplatesAbsoluteFilePath()
{
  QString retval = DataLocations::getRootConfigPath()+"/tool/TEMPLATE_configuration.xml";
  return retval;
}

void ConfigurationFileParser::saveConfiguration(Configuration& config)
{
  QDomDocument doc;
  doc.appendChild(doc.createProcessingInstruction("xml version =", "\"1.0\""));

  QDomElement configNode = doc.createElement("configuration");
  configNode.setAttribute("clinical_app", enum2string(config.mClinical_app));

  TrackersAndToolsMap::iterator it1 = config.mTrackersAndTools.begin();
  for(; it1 != config.mTrackersAndTools.end(); ++it1)
  {
    QDomElement trackerTagNode = doc.createElement("tracker");
    trackerTagNode.setAttribute("type", enum2string(it1->first));

    ToolFilesAndReferenceVector::iterator it2 = it1->second.begin();
    for(; it2 != it1->second.end(); ++it2)
    {
      QDomElement toolFileNode = doc.createElement("toolfile");
      QFileInfo info(it2->first);
      QDir configDir(config.mFileName);
//      std::cout << "Absolute config dir: " << configDir.absolutePath() << std::endl;
      toolFileNode.appendChild(doc.createTextNode(info.isDir() ? it2->first : configDir.relativeFilePath(info.filePath())));
      toolFileNode.setAttribute("reference", (it2->second ? "yes" : "no"));
      trackerTagNode.appendChild(toolFileNode);
    }
    configNode.appendChild(trackerTagNode);
  }

  doc.appendChild(configNode);

  //write to file
  QFile file(config.mFileName);
  if(!file.open(QIODevice::WriteOnly))
  {
    ssc::messageManager()->sendWarning("Could not open file "+file.fileName()+", aborting writing of config.");
    return;
  }
  QTextStream stream(&file);
  doc.save(stream, 4);
  ssc::messageManager()->sendSuccess("Configuration file "+file.fileName()+" is written.");
}

void ConfigurationFileParser::setConfigDocument(QString configAbsoluteFilePath)
{
  QFile configFile(configAbsoluteFilePath);
  if(!configFile.exists())
  {
    ssc::messageManager()->sendDebug("Configfile "+configAbsoluteFilePath+" does not exist.");
    return;
  }

  if(!mConfigureDoc.setContent(&configFile))
  {
    ssc::messageManager()->sendError("Could not set the xml content of the config file "+configAbsoluteFilePath);
    return;
  }
}

bool ConfigurationFileParser::isConfigFileValid()
{
  //there can only be one config defined in every config.xml-file, that's why we say ...item(0)
  QDomNode configNode = mConfigureDoc.elementsByTagName(mConfigTag).item(0);
  if(configNode.isNull())
  {
    //ssc::messageManager()->sendDebug("Configuration file \""+mConfigurationFilePath+"\" does not contain the tag <"+mConfigTag+">.");
    return false;
  }
  return true;
}

QString ConfigurationFileParser::getAbsoluteToolFilePath(QDomElement toolfileelement)
{
  QString absoluteToolFilePath;

  QFile configFile(mConfigurationFilePath);
  QDir configDir = QFileInfo(configFile).dir();
//  std::cout << "configDir.absolutePath(): " << configDir.absolutePath() << std::endl;

  QString relativeToolFilePath = toolfileelement.text();
//  std::cout << "relativeToolFilePath " << relativeToolFilePath << std::endl;
  if(relativeToolFilePath.isEmpty())
    return absoluteToolFilePath;

//  configDir.cd(relativeToolFilePath);
//  QFile file((configDir.absolutePath()+"/"+relativeToolFilePath));
  QFile file(configDir.absoluteFilePath(relativeToolFilePath));
  if(!file.exists())
  {
    ssc::messageManager()->sendError("Tool file "+file.fileName()+" in configuration "+mConfigurationFilePath+" does not exists. Skipping.");
  }
  QFileInfo info(file);
  if(info.isDir())
  {
//    std::cout << "IS DIR: " << absoluteToolFilePath << std::endl;
    QDir dir(info.absoluteFilePath());
    QStringList filter;
    filter << dir.dirName()+".xml";
    QStringList filepaths = dir.entryList(filter);
    if(!filepaths.isEmpty())
      absoluteToolFilePath = dir.absoluteFilePath(filter[0]);
//    else
//      std::cout << "Found no files ending with xml in dir " << dir.absolutePath() << " filter is "<< filter[0] << std::endl;
  }
  else
  {
    absoluteToolFilePath = info.absoluteFilePath();
//    std::cout << "IS FILE: "<< absoluteToolFilePath << std::endl;
  }

  std::cout << "Found toolfile " << absoluteToolFilePath << std::endl;
  return absoluteToolFilePath;
}
//----------------------------------------------------------------------------------------------------------------------

ToolFileParser::ToolFileParser(QString absoluteToolFilePath) :
    mToolFilePath(absoluteToolFilePath),
    mToolfileTag("toolfile"), mToolTag("tool"), mToolTypeTag("type"), mToolIdTag("id"), mToolNameTag("name"),
    mToolClinicalAppTag("clinical_app"),mToolGeoFileTag("geo_file"), mToolSensorTag("sensor"),
    mToolSensorTypeTag("type"), mToolSensorWirelessTag("wireless"),
    mToolSensorDOFTag("DOF"), mToolSensorPortnumberTag("portnumber"),
    mToolSensorChannelnumberTag("channelnumber"), mToolSensorReferencePointTag("reference_point"),
    mToolSensorRomFileTag("rom_file"), mToolCalibrationTag("calibration"),
    mToolCalibrationFileTag("cal_file"),
    mInstrumentTag("instrument"), mInstrumentIdTag("id"), mInstrumentScannerIdTag("scannerid")
{}

ToolFileParser::~ToolFileParser()
{}

Tool::InternalStructure ToolFileParser::getTool()
{
  Tool::InternalStructure retval;

  QFile toolFile(mToolFilePath);
  QString toolFolderAbsolutePath = QFileInfo(toolFile).dir().absolutePath()+"/";
  QDomNode toolNode = this->getToolNode(mToolFilePath);
    Tool::InternalStructure internalStructure;
    if (toolNode.isNull())
    {
      ssc::messageManager()->sendInfo("Could not read the <tool> tag of file: "+mToolFilePath+", this is not a tool file, skipping.");
      return retval;
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

    QDomElement toolClinicalAppElement = toolNode.firstChildElement(mToolClinicalAppTag);
    QString toolClinicalAppText = toolClinicalAppElement.text();
    QStringList applicationList = toolClinicalAppText.split(" ");
    foreach(QString string, applicationList)
    {
      if(string.isEmpty())
        continue;
      string = string.toLower();
      ssc::MEDICAL_DOMAIN domain = string2enum<ssc::MEDICAL_DOMAIN>(string);
      if(domain != ssc::mdCOUNT)
        internalStructure.mMedicalDomains.push_back(domain);
      else
        ssc::messageManager()->sendWarning("Did not understand the tag <clinical_app>, "+string+" is invalid in tool "+mToolFilePath);
    }

    QDomElement toolGeofileElement = toolNode.firstChildElement(mToolGeoFileTag);
    QString toolGeofileText = toolGeofileElement.text();
    if (!toolGeofileText.isEmpty())
      toolGeofileText = toolFolderAbsolutePath + toolGeofileText;
    internalStructure.mGraphicsFileName = toolGeofileText;
    QDomElement toolInstrumentElement = toolNode.firstChildElement(mInstrumentTag);
    if (toolInstrumentElement.isNull())
    {
      ssc::messageManager()->sendError("Could not find the <instrument> tag under the <tool> tag. Aborting this tool.");
      return retval;
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
      return retval;
    }
    QDomElement toolSensorTypeElement = toolSensorElement.firstChildElement(mToolSensorTypeTag);
    QString toolSensorTypeText = toolSensorTypeElement.text();
    internalStructure.mTrackerType = string2enum<ssc::TRACKING_SYSTEM>(toolSensorTypeText);

//    std::cout << "Found tracker type " << enum2string(internalStructure.mTrackerType) << " for tool " << internalStructure.mUid << std::endl;

    //    if (toolSensorTypeText.contains("polaris", Qt::CaseInsensitive))
//    {
//      if (toolSensorTypeText.contains("spectra", Qt::CaseInsensitive))
//      {
//        internalStructure.mTrackerType = ssc::tsPOLARIS_SPECTRA;
//      } else if (toolSensorTypeText.contains("vicra", Qt::CaseInsensitive))
//      {
//        internalStructure.mTrackerType = ssc::tsPOLARIS_VICRA;
//      } else
//      {
//        internalStructure.mTrackerType = ssc::tsPOLARIS;
//      }
//    } else if (toolSensorTypeText.contains("aurora", Qt::CaseInsensitive))
//    {
//      internalStructure.mTrackerType = ssc::tsAURORA;
//    } else if (toolSensorTypeText.contains("micron", Qt::CaseInsensitive))
//    {
//      internalStructure.mTrackerType = ssc::tsMICRON;
//    } else
//    {
//      internalStructure.mTrackerType = ssc::tsNONE;
//    }

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
      toolSensorRomFileText = toolFolderAbsolutePath + toolSensorRomFileText;
    internalStructure.mSROMFilename = toolSensorRomFileText;

    QDomElement toolCalibrationElement = toolNode.firstChildElement(mToolCalibrationTag);
    if (toolCalibrationElement.isNull())
    {
      ssc::messageManager()->sendError("Could not find the <calibration> tag under the <tool> tag. Aborting this tool.");
      return retval;
    }
    QDomElement toolCalibrationFileElement = toolCalibrationElement.firstChildElement(mToolCalibrationFileTag);
    QString toolCalibrationFileText = toolCalibrationFileElement.text();
    if (!toolCalibrationFileText.isEmpty())
      toolCalibrationFileText = toolFolderAbsolutePath + toolCalibrationFileText;
    internalStructure.mCalibrationFilename = toolCalibrationFileText;
    internalStructure.mCalibration = this->readCalibrationFile(internalStructure.mCalibrationFilename);

    internalStructure.mTransformSaveFileName = ""; //TODO
    internalStructure.mLoggingFolderName = ""; //TODO
    retval = internalStructure;

  return retval;
}

QDomNode ToolFileParser::getToolNode(QString toolAbsoluteFilePath)
{
  QDomNode retval;
  QFile toolFile(toolAbsoluteFilePath);
  if (!mToolDoc.setContent(&toolFile))
  {
    ssc::messageManager()->sendError("Could not set the xml content of the tool file "+toolAbsoluteFilePath);
    return retval;
  }
  //there can only be one tool defined in every tool.xml-file, that's why we say ...item(0)
  retval = mToolDoc.elementsByTagName(mToolTag).item(0);
  return retval;
}

QString ToolFileParser::getTemplatesAbsoluteFilePath()
{
  QString retval = DataLocations::getRootConfigPath()+"/tool/TEMPLATE_tool.xml";
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
