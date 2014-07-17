// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxToolConfigurationParser.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include "cxReporter.h"
#include "cxTypeConversions.h"
#include "cxEnumConverter.h"
#include "cxDataLocations.h"
#include "cxFrame3D.h"
#include "cxTransformFile.h"

namespace cx
{

ConfigurationFileParser::ConfigurationFileParser(QString absoluteConfigFilePath, QString loggingFolder) :
				mConfigurationFilePath(absoluteConfigFilePath), mLoggingFolder(loggingFolder), mConfigTag(
								"configuration"), mConfigTrackerTag("tracker"), mConfigTrackerToolFile("toolfile"), mTypeAttribute(
								"type"), mClinicalAppAttribute("clinical_app"), mReferenceAttribute("reference")
{
	this->setConfigDocument(mConfigurationFilePath);
}

ConfigurationFileParser::~ConfigurationFileParser()
{
}

CLINICAL_APPLICATION ConfigurationFileParser::getApplicationapplication()
{
	CLINICAL_APPLICATION retval = mdCOUNT;

	if (!this->isConfigFileValid())
		return retval;

	QDomNode configNode = mConfigureDoc.elementsByTagName(mConfigTag).at(0);
	QString applicationapplication = configNode.toElement().attribute(mClinicalAppAttribute);
	retval = string2enum<CLINICAL_APPLICATION>(applicationapplication);

	return retval;
}

std::vector<IgstkTracker::InternalStructure> ConfigurationFileParser::getTrackers()
{
	std::vector<IgstkTracker::InternalStructure> retval;

	if (!this->isConfigFileValid())
		return retval;

	QDomNodeList trackerNodes = mConfigureDoc.elementsByTagName(mConfigTrackerTag);
	for (int i = 0; i < trackerNodes.count(); ++i)
	{
		IgstkTracker::InternalStructure internalStructure;
		QString trackerType = trackerNodes.at(i).toElement().attribute(mTypeAttribute);
		internalStructure.mType = string2enum<TRACKING_SYSTEM>(trackerType);
		internalStructure.mLoggingFolderName = mLoggingFolder;

//    std::cout << "In configfile " << mConfigurationFilePath << " found tracker type " << enum2string(internalStructure.mType) << std::endl;
		retval.push_back(internalStructure);
	}

	if (retval.size() > 1)
		reportError(
						"Config file " + mConfigurationFilePath
										+ " has a invalid number of tracking systems, we only support 1 tracking system atm!");

	return retval;
}

std::vector<QString> ConfigurationFileParser::getAbsoluteToolFilePaths()
{
//	std::cout << "*******************************************************" << std::endl;
//	std::cout << "*** ConfigurationFileParser::getAbsoluteToolFilePaths()" << std::endl;
//	std::cout << "*******************************************************" << std::endl;
	std::vector<QString> retval;

	if (!this->isConfigFileValid())
		return retval;

	QDomNodeList toolFileNodes = mConfigureDoc.elementsByTagName(mConfigTrackerToolFile);
	for (int i = 0; i < toolFileNodes.count(); ++i)
	{
		QString absoluteToolFilePath = this->getAbsoluteToolFilePath(toolFileNodes.at(i).toElement());
		if (absoluteToolFilePath.isEmpty())
			continue;

		retval.push_back(absoluteToolFilePath);
	}

	return retval;
}

QString ConfigurationFileParser::getAbsoluteReferenceFilePath()
{
	QString retval;

	if (!this->isConfigFileValid())
		return retval;

//  QFile configFile(mConfigurationFilePath);
//  QString configFolderAbsolutePath = QFileInfo(configFile).dir().absolutePath()+"/";
//  std::cout << "configFolderAbsolutePath " << configFolderAbsolutePath << std::endl;

	QDomNodeList toolFileNodes = mConfigureDoc.elementsByTagName(mConfigTrackerToolFile);
	for (int i = 0; i < toolFileNodes.count(); ++i)
	{
		QString reference = toolFileNodes.at(i).toElement().attribute(mReferenceAttribute);
		if (reference.contains("yes", Qt::CaseInsensitive))
		{
//      std::cout << "Found yes..." << std::endl;
			retval = this->getAbsoluteToolFilePath(toolFileNodes.at(i).toElement());
		}
	}
	return retval;
}

QString ConfigurationFileParser::getTemplatesAbsoluteFilePath()
{
	QString retval = DataLocations::getRootConfigPath() + "/tool/TEMPLATE_configuration.xml";
	return retval;
}

QString ConfigurationFileParser::convertToRelativeToolFilePath(QString configFilename, QString absoluteToolFilePath)
{
	QDir configDir = QFileInfo(configFilename).dir();
	std::map<int, QString> candicates;

	foreach (QString configroot, DataLocations::getRootConfigPaths())
	{
		QString relConfigPath = QDir(configroot).relativeFilePath(configDir.path());

		foreach (QString configroot2, DataLocations::getRootConfigPaths())
		{
			QString absConfigPath = configroot2 + "/" + relConfigPath;
			QString relToolFilePath = QDir(absConfigPath).relativeFilePath(absoluteToolFilePath);
			relToolFilePath = compactVirtualPath(relToolFilePath);
			candicates[relToolFilePath.size()] = relToolFilePath;
		}
	}

	// return smallest result
	return candicates.begin()->second;
}

void ConfigurationFileParser::saveConfiguration(Configuration& config)
{
	QDomDocument doc;
	doc.appendChild(doc.createProcessingInstruction("xml version =", "\"1.0\""));

	QDomElement configNode = doc.createElement("configuration");
	configNode.setAttribute("clinical_app", enum2string(config.mClinical_app));

	TrackersAndToolsMap::iterator it1 = config.mTrackersAndTools.begin();
	for (; it1 != config.mTrackersAndTools.end(); ++it1)
	{
		QString trackerType = enum2string(it1->first);
		QDomElement trackerTagNode = doc.createElement("tracker");
		trackerTagNode.setAttribute("type", trackerType);

		ToolFilesAndReferenceVector::iterator it2 = it1->second.begin();
		for (; it2 != it1->second.end(); ++it2)
		{
			QString absoluteToolFilePath = it2->first;
//			QString relativeToolFilePath = it2->first;
			//std::cout << "relativeToolFilePath" << relativeToolFilePath << std::endl;
			QString relativeToolFilePath = convertToRelativeToolFilePath(config.mFileName, absoluteToolFilePath);

//			QFileInfo configFileInfo(config.mFileName);
//			const QDir configDir(configFileInfo.absolutePath());

//			QFileInfo toolFileInfo(configDir, relativeToolFilePath);
//			const QDir toolFileDir(toolFileInfo.absolutePath());

//			QString absoluteToolFilePath = toolFileDir.absoluteFilePath(toolFileInfo.fileName());
			//      std::cout << "absoluteToolFilePath " << absoluteToolFilePath << std::endl;
			ToolFileParser toolparser(absoluteToolFilePath);
			QString toolTrackerType = enum2string(toolparser.getTool().mTrackerType);
			//      std::cout << "toolTrackerType " << toolTrackerType << " trackerType " << trackerType << std::endl;
			if (!trackerType.contains(enum2string(toolparser.getTool().mTrackerType), Qt::CaseInsensitive))
			{
				reportWarning("When saving configuration, skipping tool " + relativeToolFilePath + " of type "
												+ toolTrackerType + " because tracker is set to " + trackerType);
				continue;
			}

			QDomElement toolFileNode = doc.createElement("toolfile");
			toolFileNode.appendChild(doc.createTextNode(relativeToolFilePath));
//											toolFileInfo.isDir() ?
//															it2->first :
//															configDir.relativeFilePath(toolFileInfo.filePath())));
			toolFileNode.setAttribute("reference", (it2->second ? "yes" : "no"));
			trackerTagNode.appendChild(toolFileNode);
		}
		configNode.appendChild(trackerTagNode);
	}

	doc.appendChild(configNode);

	//write to file
	QFile file(config.mFileName);
	QDir().mkpath(QFileInfo(config.mFileName).absolutePath());

	if (!file.open(QIODevice::WriteOnly))
	{
		reportWarning("Could not open file " + file.fileName() + ", aborting writing of config.");
		return;
	}
	QTextStream stream(&file);
	doc.save(stream, 4);
	reportSuccess("Configuration file " + file.fileName() + " is written.");
}

void ConfigurationFileParser::setConfigDocument(QString configAbsoluteFilePath)
{
	QFile configFile(configAbsoluteFilePath);
	if (!configFile.exists())
	{
//    reportDebug("Configfile "+configAbsoluteFilePath+" does not exist.");
		return;
	}

	if (!mConfigureDoc.setContent(&configFile))
	{
		reportError("Could not set the xml content of the config file " + configAbsoluteFilePath);
		return;
	}
}

bool ConfigurationFileParser::isConfigFileValid()
{
	//there can only be one config defined in every config.xml-file, that's why we say ...item(0)
	QDomNode configNode = mConfigureDoc.elementsByTagName(mConfigTag).item(0);
	if (configNode.isNull())
	{
		//reportDebug("Configuration file \""+mConfigurationFilePath+"\" does not contain the tag <"+mConfigTag+">.");
		return false;
	}
	return true;
}

QString ConfigurationFileParser::findXmlFileWithDirNameInPath(QString path)
{
	QDir dir(path);
	QStringList filter;
	filter << dir.dirName() + ".xml";
	QStringList filepaths = dir.entryList(filter);
	if (!filepaths.isEmpty())
		return dir.absoluteFilePath(filter[0]);
	return "";
}

/** Remove ".." elements in filepath, ignoring existence of the input
  * path. This replaces QFileInfo::canonicalPath(), which requires
  * all intervening dirs to exist.
  */
QString ConfigurationFileParser::compactVirtualPath(QString filepath)
{
	// remove ../ elements
	QStringList elems = filepath.split("/");
	while (elems.count(".."))
	{
		int index = elems.indexOf("..");
		if (index<=0)
			break;
		elems.removeAt(index-1); // remove prefix dir
		elems.removeAt(index-1); // remove ..
	}
	return elems.join("/");
}

QString ConfigurationFileParser::searchForExistingToolFilePath(QString relativeToolFilePath)
{
//	std::cout << "QString ConfigurationFileParser::searchForExistingToolFilePath(QString relativeToolFilePath)" << std::endl;
	// find rconfigfilepath relative to all configroots
	// pick shortest rconfigfilepath
	// find toolfilepath as shortest of configroot + rconfigfilepath + rtoolfilepath

	QDir configDir = QFileInfo(mConfigurationFilePath).dir();
//	std::cout <<  "      == configdir " << configDir.absolutePath() << std::endl;

	foreach (QString configroot, DataLocations::getRootConfigPaths())
	{
//		std::cout << "      == configroot " << configroot << std::endl;
		QString relConfigFilePath = QDir(configroot).relativeFilePath(configDir.path());
//		std::cout << "      == relConfigFilePath " << relConfigFilePath << std::endl;

		foreach (QString configroot2, DataLocations::getRootConfigPaths())
		{
			QString toolFilePath = configroot2 + "/" + relConfigFilePath + "/" + relativeToolFilePath;
//			std::cout << "      ==== configroot2 " << configroot2 << std::endl;
//			std::cout << "      ==== toolFilePath " << toolFilePath << std::endl;
			toolFilePath = this->compactVirtualPath(toolFilePath);
//			std::cout << "      ==== toolFilePath mod " << toolFilePath << std::endl;
//			std::cout << "      ==== ctoolFilePath " << QFileInfo(toolFilePath).canonicalFilePath() << std::endl;
//			std::cout << "      ==== exists " << QFileInfo(toolFilePath).exists() << std::endl;
			if (QFileInfo(toolFilePath).exists())
				return QFileInfo(toolFilePath).canonicalFilePath();
		}
	}

	return "";
}

QString ConfigurationFileParser::getAbsoluteToolFilePath(QDomElement toolfileelement)
{
	QString relativeToolFilePath = toolfileelement.text();
//	std::cout << "   ============================================================" << std::endl;
//	std::cout << QString("   ===== QString ConfigurationFileParser::getAbsoluteToolFilePath(%1) ").arg(relativeToolFilePath) << std::endl;
	if (relativeToolFilePath.isEmpty())
		return "";

	QString absoluteToolFilePath = this->searchForExistingToolFilePath(relativeToolFilePath);

	QFileInfo info(absoluteToolFilePath);
	if (!info.exists())
		reportError(QString("Tool file %1 in configuration %2 not found. Skipping.")
					.arg(info.fileName())
					.arg(mConfigurationFilePath));

//	std::cout << QString("  [%1] ").arg(relativeToolFilePath) << std::endl;
//	std::cout << QString("       -> ") << std::endl;
//	std::cout << QString("            [%2] ").arg(absoluteToolFilePath) << std::endl;
//	std::cout << "   exist: " << info.exists() << ", dir: " << info.isDir() << std::endl;

	if (info.isDir())
		absoluteToolFilePath = this->findXmlFileWithDirNameInPath(absoluteToolFilePath);
	return absoluteToolFilePath;
}
//----------------------------------------------------------------------------------------------------------------------

ToolFileParser::ToolFileParser(QString absoluteToolFilePath, QString loggingFolder) :
				mToolFilePath(absoluteToolFilePath), mLoggingFolder(loggingFolder), mToolTag("tool"), mToolTypeTag(
								"type"), mToolIdTag("id"), mToolNameTag("name"), mToolDescriptionTag("description"), mToolManufacturerTag(
								"manufacturer"), mToolClinicalAppTag("clinical_app"), mToolGeoFileTag("geo_file"), mToolPicFileTag(
								"pic_file"), mToolDocFileTag("doc_file"), mToolInstrumentTag("instrument"), mToolInstrumentTypeTag(
								"type"), mToolInstrumentIdTag("id"), mToolInstrumentNameTag("name"), mToolInstrumentManufacturerTag(
								"manufacturer"), mToolInstrumentScannerIdTag("scannerid"), mToolInstrumentDescriptionTag(
								"description"), mToolSensorTag("sensor"), mToolSensorTypeTag("type"), mToolSensorIdTag(
								"id"), mToolSensorNameTag("name"), mToolSensorWirelessTag("wireless"), mToolSensorDOFTag(
								"DOF"), mToolSensorPortnumberTag("portnumber"), mToolSensorChannelnumberTag(
								"channelnumber"), mToolSensorReferencePointTag("reference_point"), mToolSensorManufacturerTag(
								"manufacturer"), mToolSensorDescriptionTag("description"), mToolSensorRomFileTag(
								"rom_file"), mToolCalibrationTag("calibration"), mToolCalibrationFileTag("cal_file")
{
}

ToolFileParser::~ToolFileParser()
{
}

IgstkTool::InternalStructure ToolFileParser::getTool()
{
	IgstkTool::InternalStructure retval;

	QFile toolFile(mToolFilePath);
	QString toolFolderAbsolutePath = QFileInfo(toolFile).dir().absolutePath() + "/";
	QDomNode toolNode = this->getToolNode(mToolFilePath);
	IgstkTool::InternalStructure internalStructure;
	if (toolNode.isNull())
	{
		report(
						"Could not read the <tool> tag of file: " + mToolFilePath
										+ ", this is not a tool file, skipping.");
		return retval;
	}

	QDomElement toolTypeElement = toolNode.firstChildElement(mToolTypeTag);
	QString toolTypeText = toolTypeElement.text();

	internalStructure.mIsReference = toolTypeText.contains("reference", Qt::CaseInsensitive);
	internalStructure.mIsPointer = toolTypeText.contains("pointer", Qt::CaseInsensitive);
	internalStructure.mIsProbe = toolTypeText.contains("usprobe", Qt::CaseInsensitive);

//    if (toolTypeText.contains("reference", Qt::CaseInsensitive))
//    {
//      internalStructure.mType = Tool::TOOL_REFERENCE;
//    } else if (toolTypeText.contains("pointer", Qt::CaseInsensitive))
//    {
//      internalStructure.mType = Tool::TOOL_POINTER;
//    } else if (toolTypeText.contains("usprobe", Qt::CaseInsensitive))
//    {
//      internalStructure.mType = Tool::TOOL_US_PROBE;
//    } else
//    {
//      internalStructure.mType = Tool::TOOL_NONE;
//    }

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
		if (string.isEmpty())
			continue;
		string = string.toLower();
		CLINICAL_APPLICATION application = string2enum<CLINICAL_APPLICATION>(string);
		if (application != mdCOUNT)
			internalStructure.mClinicalApplications.push_back(application);
		else
			reportWarning(
							"Did not understand the tag <clinical_app>, " + string + " is invalid in tool "
											+ mToolFilePath);
	}

	QDomElement toolGeofileElement = toolNode.firstChildElement(mToolGeoFileTag);
	QString toolGeofileText = toolGeofileElement.text();
	if (!toolGeofileText.isEmpty())
		toolGeofileText = toolFolderAbsolutePath + toolGeofileText;
	internalStructure.mGraphicsFileName = toolGeofileText;

	QDomElement toolPicfileElement = toolNode.firstChildElement(mToolPicFileTag);
	QString toolPicfileText = toolPicfileElement.text();
	if (!toolPicfileText.isEmpty())
		toolPicfileText = toolFolderAbsolutePath + toolPicfileText;
	internalStructure.mPictureFileName = toolPicfileText;

	QDomElement toolInstrumentElement = toolNode.firstChildElement(mToolInstrumentTag);
	if (toolInstrumentElement.isNull())
	{
		reportError(
						"Could not find the <instrument> tag under the <tool> tag. Aborting this tool.");
		return retval;
	}
	QDomElement toolInstrumentIdElement = toolInstrumentElement.firstChildElement(mToolInstrumentIdTag);
	QString toolInstrumentIdText = toolInstrumentIdElement.text();
	internalStructure.mInstrumentId = toolInstrumentIdText;

	QDomElement toolInstrumentScannerIdElement = toolInstrumentElement.firstChildElement(mToolInstrumentScannerIdTag);
	QString toolInstrumentScannerIdText = toolInstrumentScannerIdElement.text();
	internalStructure.mInstrumentScannerId = toolInstrumentScannerIdText;

	QDomElement toolSensorElement = toolNode.firstChildElement(mToolSensorTag);
	if (toolSensorElement.isNull())
	{
		reportError("Could not find the <sensor> tag under the <tool> tag. Aborting this tool.");
		return retval;
	}
	QDomElement toolSensorTypeElement = toolSensorElement.firstChildElement(mToolSensorTypeTag);
	QString toolSensorTypeText = toolSensorTypeElement.text();
	internalStructure.mTrackerType = string2enum<TRACKING_SYSTEM>(toolSensorTypeText);

	QDomElement toolSensorWirelessElement = toolSensorElement.firstChildElement(mToolSensorWirelessTag);
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

	QDomElement toolSensorPortnumberElement = toolSensorElement.firstChildElement(mToolSensorPortnumberTag);
	QString toolSensorPortnumberText = toolSensorPortnumberElement.text();
	internalStructure.mPortNumber = toolSensorPortnumberText.toInt();

	QDomElement toolSensorChannelnumberElement = toolSensorElement.firstChildElement(mToolSensorChannelnumberTag);
	QString toolSensorChannelnumberText = toolSensorChannelnumberElement.text();
	internalStructure.mChannelNumber = toolSensorChannelnumberText.toInt();

	QDomNodeList toolSensorReferencePointList = toolSensorElement.elementsByTagName(mToolSensorReferencePointTag);
	for (int j = 0; j < toolSensorReferencePointList.count(); j++)
	{
		QDomNode node = toolSensorReferencePointList.item(j);
		if (!node.hasAttributes())
		{
			reportWarning("Found reference point without id attribute. Skipping.");
			continue;
		}
		bool ok;
		int id = node.toElement().attribute("id").toInt(&ok);
		if (!ok)
		{
			reportWarning("Attribute id of a reference point was not an int. Skipping.");
			continue;
		}
		QString toolSensorReferencePointText = node.toElement().text();
		Vector3D vector = Vector3D::fromString(toolSensorReferencePointText);
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
		reportError(
						"Could not find the <calibration> tag under the <tool> tag. Aborting this tool.");
		return retval;
	}
	QDomElement toolCalibrationFileElement = toolCalibrationElement.firstChildElement(mToolCalibrationFileTag);
	QString toolCalibrationFileText = toolCalibrationFileElement.text();
	if (!toolCalibrationFileText.isEmpty())
		toolCalibrationFileText = toolFolderAbsolutePath + toolCalibrationFileText;
	internalStructure.mCalibrationFilename = toolCalibrationFileText;
	internalStructure.mCalibration = this->readCalibrationFile(internalStructure.mCalibrationFilename);

	internalStructure.mTransformSaveFileName = mLoggingFolder;
	internalStructure.mLoggingFolderName = mLoggingFolder;
	retval = internalStructure;

	return retval;
}

QDomNode ToolFileParser::getToolNode(QString toolAbsoluteFilePath)
{
	QDomNode retval;
	QFile toolFile(toolAbsoluteFilePath);
	if (!mToolDoc.setContent(&toolFile))
	{
		reportError("Could not set the xml content of the tool file " + toolAbsoluteFilePath);
		return retval;
	}
	//there can only be one tool defined in every tool.xml-file, that's why we say ...item(0)
	retval = mToolDoc.elementsByTagName(mToolTag).item(0);
	return retval;
}

QString ToolFileParser::getTemplatesAbsoluteFilePath()
{
	QString retval = DataLocations::getRootConfigPath() + "/tool/TEMPLATE_tool.xml";
	return retval;
}

igstk::Transform ToolFileParser::readCalibrationFile(QString absoluteFilePath)
{
	igstk::Transform retval;

	bool ok = true;
	TransformFile file(absoluteFilePath);
	Transform3D M = file.read(&ok);

	if (ok)
	{
		M = Frame3D::create(M).transform(); // clean rotational parts, transform should now be pure rotation+translation
		retval.ImportTransform(*M.getVtkMatrix());
	}

	return retval;
}

//----------------------------------------------------------------------------------------------------------------------

}//namespace cx
