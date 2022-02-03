/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolConfigurationParser.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxEnumConversion.h"
#include "cxDataLocations.h"
#include "cxProfile.h"
#include "cxFrame3D.h"
#include "cxTransformFile.h"

namespace cx
{

// names of necessary tags in the configuration file
#define CONFIG_TAG "configuration"
#define CONFIG_TRACKER_TAG "tracker"
#define CONFIG_TRACKER_TOOL_FILE "toolfile"
#define CONFIG_TRACKINGSYSTEMIMPLEMENTATION_TAG "trackingsystemimplementation"

// names of necessary attributes in the configuration file
#define TYPE_ATTRIBUTE "type"
#define CLINICAL_APP_ATTRIBUTE "clinical_app"
#define REFERENCE_ATTRIBUTE "reference"
#define OPENIGTLINK_TRANSFORM_ID_ATTRIBUTE "openigtlinktransformid"
#define OPENIGTLINK_IMAGE_ID_ATTRIBUTE "openigtlinkimageid"
#define OPENIGTLINK_APPLY_REF_TO_TOOLS_ATTRIBUTE "openigtlinkApplyRefToTools"

ConfigurationFileParser::ConfigurationFileParser(QString absoluteConfigFilePath, QString loggingFolder) :
				mConfigurationFilePath(absoluteConfigFilePath), mLoggingFolder(loggingFolder)
{
	this->setConfigDocument(mConfigurationFilePath);
}

ConfigurationFileParser::~ConfigurationFileParser()
{
}

QString ConfigurationFileParser::getApplicationapplication()
{
	if (!this->isConfigFileValid())
		return "";

	QDomNode configNode = mConfigureDoc.elementsByTagName(CONFIG_TAG).at(0);
	QString retval = configNode.toElement().attribute(CLINICAL_APP_ATTRIBUTE);
	return retval;
}

QString ConfigurationFileParser::getTrackingSystemImplementation()
{
	QString retval;

	QDomNodeList trackingsystemImplementationNodes = mConfigureDoc.elementsByTagName(CONFIG_TRACKINGSYSTEMIMPLEMENTATION_TAG);
	for (int i = 0; i < trackingsystemImplementationNodes.count(); ++i)
	{
		retval = trackingsystemImplementationNodes.at(i).toElement().attribute(TYPE_ATTRIBUTE);
	}

	if (trackingsystemImplementationNodes.count() == 0)
	{
		//CX_LOG_DEBUG() << "Cannot find " << CONFIG_TRACKINGSYSTEMIMPLEMENTATION_TAG << " tag. Selecting " << TRACKING_SYSTEM_IMPLEMENTATION_IGSTK;
		retval = TRACKING_SYSTEM_IMPLEMENTATION_IGSTK;//Revert to igstk implementation for old config files
	}
	else if(trackingsystemImplementationNodes.count() > 1)
	{
		CX_LOG_ERROR() << "ConfigurationFileParser::getTrackingSystemImplementation(): Config file: " << mConfigurationFilePath
									 << " has more the one tracking system implementation. Only one is currently supported.";
	}

	return retval;
}

std::vector<ToolFileParser::TrackerInternalStructure> ConfigurationFileParser::getTrackers()
{
    std::vector<ToolFileParser::TrackerInternalStructure> retval;

	if (!this->isConfigFileValid())
		return retval;

	QDomNodeList trackerNodes = mConfigureDoc.elementsByTagName(CONFIG_TRACKER_TAG);
	for (int i = 0; i < trackerNodes.count(); ++i)
	{
        ToolFileParser::TrackerInternalStructure internalStructure;
		QString trackerType = trackerNodes.at(i).toElement().attribute(TYPE_ATTRIBUTE);
		internalStructure.mType = string2enum<TRACKING_SYSTEM>(trackerType);
		internalStructure.mLoggingFolderName = mLoggingFolder;

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
	std::vector<QString> retval;

	if (!this->isConfigFileValid())
		return retval;

	QDomNodeList toolFileNodes = mConfigureDoc.elementsByTagName(CONFIG_TRACKER_TOOL_FILE);
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

	QDomNodeList toolFileNodes = mConfigureDoc.elementsByTagName(CONFIG_TRACKER_TOOL_FILE);
	for (int i = 0; i < toolFileNodes.count(); ++i)
	{
		QString reference = toolFileNodes.at(i).toElement().attribute(REFERENCE_ATTRIBUTE);
		if (reference.contains("yes", Qt::CaseInsensitive))
		{
//      std::cout << "Found yes..." << std::endl;
			retval = this->getAbsoluteToolFilePath(toolFileNodes.at(i).toElement());
		}
	}
	return retval;
}

std::vector<ConfigurationFileParser::ToolStructure> ConfigurationFileParser::getToolListWithMetaInformation()
{
	std::vector<ToolStructure> retval;

	if (!this->isConfigFileValid())
		return retval;
	bool applyRefToTools = this->getApplyRefToTools();

	QDomNodeList toolFileNodes = mConfigureDoc.elementsByTagName(CONFIG_TRACKER_TOOL_FILE);
	for (int i = 0; i < toolFileNodes.count(); ++i)
	{
		ToolStructure toolStructure;
		toolStructure.mAbsoluteToolFilePath = this->getAbsoluteToolFilePath(toolFileNodes.at(i).toElement());
		toolStructure.mOpenIGTLinkTransformId = toolFileNodes.at(i).toElement().attribute(OPENIGTLINK_TRANSFORM_ID_ATTRIBUTE);
		toolStructure.mOpenIGTLinkImageId = toolFileNodes.at(i).toElement().attribute(OPENIGTLINK_IMAGE_ID_ATTRIBUTE);
		toolStructure.mApplyRefToTool = applyRefToTools;

		QString reference = toolFileNodes.at(i).toElement().attribute(REFERENCE_ATTRIBUTE);
		if (reference.contains("yes", Qt::CaseInsensitive))
			toolStructure.mReference = true;
		else
			toolStructure.mReference = false;
		retval.push_back(toolStructure);
	}
	return retval;
}

QString ConfigurationFileParser::getTemplatesAbsoluteFilePath()
{
	QString retval = DataLocations::getRootConfigPath() + "/tool/TEMPLATE_configuration.xml";
	return retval;
}

bool ConfigurationFileParser::getApplyRefToTools()
{
	bool retval = false;
	if (!this->isConfigFileValid())
		return retval;

	QDomNodeList trackingsystemImplementationNodes = mConfigureDoc.elementsByTagName(CONFIG_TRACKINGSYSTEMIMPLEMENTATION_TAG);
	for (int i = 0; i < trackingsystemImplementationNodes.count(); ++i)
	{
		QString applyRef = trackingsystemImplementationNodes.at(i).toElement().attribute(OPENIGTLINK_APPLY_REF_TO_TOOLS_ATTRIBUTE);
		if (applyRef.contains("yes", Qt::CaseInsensitive))
			retval = true;
	}
	//Is it neccesary to check for number of tracking system implementations here?
	//This is done in ConfigurationFileParser::getTrackingSystemImplementation()
	if(trackingsystemImplementationNodes.count() > 1)
	{
		CX_LOG_ERROR() << "ConfigurationFileParser::getApplyRefToTools(): Config file: " << mConfigurationFilePath
					   << " has more the one tracking system implementation. Only one is currently supported.";
	}
	return retval;
}

QString ConfigurationFileParser::convertToRelativeToolFilePath(QString configFilename, QString absoluteToolFilePath)
{
	foreach (QString root, profile()->getAllRootConfigPaths())
	{
		QString configPath = getToolPathFromRoot(root);
		if (!absoluteToolFilePath.contains(configPath))
			continue;
		absoluteToolFilePath.replace(configPath, "");
		if (absoluteToolFilePath.startsWith("/"))
			absoluteToolFilePath.remove(0, 1);
		return absoluteToolFilePath;
	}

	// file not in any of the standard locations: return absolute
	return absoluteToolFilePath;
}

QString ConfigurationFileParser::getToolPathFromRoot(QString root)
{
	return root + "/tool/Tools/";
}

void ConfigurationFileParser::saveConfiguration(Configuration& config)
{
	QDomDocument doc;
	doc.appendChild(doc.createProcessingInstruction("xml version =", "\"1.0\""));

	QDomElement configNode = doc.createElement(CONFIG_TAG);
	configNode.setAttribute(CLINICAL_APP_ATTRIBUTE, config.mClinical_app);

	QDomElement trackingsystemImplementationNode = doc.createElement(CONFIG_TRACKINGSYSTEMIMPLEMENTATION_TAG);
	trackingsystemImplementationNode.setAttribute(TYPE_ATTRIBUTE, config.mTrackingSystemImplementation);

	if (config.mTrackingSystemImplementation.contains(TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK, Qt::CaseInsensitive))
		trackingsystemImplementationNode.setAttribute(OPENIGTLINK_APPLY_REF_TO_TOOLS_ATTRIBUTE, (config.mApplyRefToTools ? "yes" : "no"));

	configNode.appendChild(trackingsystemImplementationNode);

	TrackersAndToolsMap::iterator it1 = config.mTrackersAndTools.begin();
	for (; it1 != config.mTrackersAndTools.end(); ++it1)
	{
		QString trackingSystemName = enum2string(it1->first);
		if(trackingSystemName.isEmpty())
		{
			CX_LOG_WARNING() << "trackingSystemName is empty.";
			trackingSystemName="";
		}
		QDomElement trackerTagNode = doc.createElement(CONFIG_TRACKER_TAG);
		trackerTagNode.setAttribute(TYPE_ATTRIBUTE, trackingSystemName);

		ToolStructureVector::iterator it2 = it1->second.begin();
		for (; it2 != it1->second.end(); ++it2)
		{
			QString absoluteToolFilePath = it2->mAbsoluteToolFilePath;
			QString relativeToolFilePath = convertToRelativeToolFilePath(config.mFileName, absoluteToolFilePath);

			ToolFileParser toolparser(absoluteToolFilePath);
			QString toolTrackerType = enum2string(toolparser.getTool()->mTrackerType);

			if (!trackingSystemName.contains(enum2string(toolparser.getTool()->mTrackerType), Qt::CaseInsensitive))
			{
				reportWarning("When saving configuration, skipping tool " + relativeToolFilePath + " of type "
												+ toolTrackerType + " because trackingSystemName is set to " + trackingSystemName);
				continue;
			}

			QDomElement toolFileNode = doc.createElement(CONFIG_TRACKER_TOOL_FILE);
			toolFileNode.appendChild(doc.createTextNode(relativeToolFilePath));
			createToolFileNode(it2, toolFileNode, toolparser);
			trackerTagNode.appendChild(toolFileNode);
		}
		trackingsystemImplementationNode.appendChild(trackerTagNode);
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

void ConfigurationFileParser::createToolFileNode(ToolStructureVector::iterator iter, QDomElement &toolFileNode, ToolFileParser &toolparser)
{
	toolFileNode.setAttribute(REFERENCE_ATTRIBUTE, (iter->mReference ? "yes" : "no"));
	//Use OpenIGTLink id values from tool config file if present
	QString transformId = iter->mOpenIGTLinkTransformId;
	QString imageId = iter->mOpenIGTLinkImageId;
	//Otherwise use id values from tool  file
	if(transformId.isEmpty())
		transformId = toolparser.getTool()->mOpenigtlinkTransformId;
	if(imageId.isEmpty())
		imageId = toolparser.getTool()->mOpenigtlinkImageId;
	if(!transformId.isEmpty())
		toolFileNode.setAttribute(OPENIGTLINK_TRANSFORM_ID_ATTRIBUTE, transformId);
	if(!imageId.isEmpty())
		toolFileNode.setAttribute(OPENIGTLINK_IMAGE_ID_ATTRIBUTE, imageId);
}

void ConfigurationFileParser::setConfigDocument(QString configAbsoluteFilePath)
{
	QFile configFile(configAbsoluteFilePath);
	if (!configFile.exists())
	{
//    reportDebug("Configfile "+configAbsoluteFilePath+" does not exist.");
		return;
	}

	QString errorMessage;
	int errorInLine = 0;
	if (!mConfigureDoc.setContent(&configFile, &errorMessage, &errorInLine))
	{
		reportError("Could not set the xml content of the config file " + configAbsoluteFilePath);
		CX_LOG_ERROR() << "Qt error message: " << errorMessage << " in line: " << errorInLine;
		return;
	}
}

bool ConfigurationFileParser::isConfigFileValid()
{
	//there can only be one config defined in every config.xml-file, that's why we say ...item(0)
	QDomNode configNode = mConfigureDoc.elementsByTagName(CONFIG_TAG).item(0);
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

QString ConfigurationFileParser::searchForExistingToolFilePath(QString relativeToolFilePath)
{
	// remove old-style paths (<= v3.7.0)
	relativeToolFilePath.replace("../Tools/", "");

	foreach (QString root, profile()->getAllRootConfigPaths())
	{
		QString configPath = this->getToolPathFromRoot(root);
		QFileInfo guess(configPath + "/" + relativeToolFilePath);
		if (guess.exists())
			return guess.canonicalFilePath();
	}
	return "";
}

QString ConfigurationFileParser::getAbsoluteToolFilePath(QDomElement toolfileelement)
{
	QString relativeToolFilePath = toolfileelement.text();
	if (relativeToolFilePath.isEmpty())
		return "";

	QString absoluteToolFilePath = this->searchForExistingToolFilePath(relativeToolFilePath);

	QFileInfo info(absoluteToolFilePath);
	if (!info.exists())
		reportError(QString("Tool file %1 in configuration %2 not found. Skipping.")
					.arg(relativeToolFilePath)
					.arg(mConfigurationFilePath));

	if (info.isDir())
		absoluteToolFilePath = this->findXmlFileWithDirNameInPath(absoluteToolFilePath);
	return absoluteToolFilePath;
}
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------

}//namespace cx
