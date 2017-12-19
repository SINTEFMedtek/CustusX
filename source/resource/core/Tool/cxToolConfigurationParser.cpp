/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxToolConfigurationParser.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxEnumConverter.h"
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
#define CONFIG_TRACKINGSYSTEM_TAG "trackingsystem"

// names of necessary attributes in the configuration file
#define TYPE_ATTRIBUTE "type"
#define CLINICAL_APP_ATTRIBUTE "clinical_app"
#define REFERENCE_ATTRIBUTE "reference"
#define OPENIGTLINK_TRANSFORM_ID_ATTRIBUTE "openigtlinktransformid"
#define OPENIGTLINK_IMAGE_ID_ATTRIBUTE "openigtlinkimageid"

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

QString ConfigurationFileParser::getTrackingSystem()
{
	QString retval;

	QDomNodeList trackingsystemNodes = mConfigureDoc.elementsByTagName(CONFIG_TRACKINGSYSTEM_TAG);
	for (int i = 0; i < trackingsystemNodes.count(); ++i)
	{
		retval = trackingsystemNodes.at(i).toElement().attribute(TYPE_ATTRIBUTE);
	}

	if (trackingsystemNodes.count() == 0)
	{
		retval = "igstk";//Revert to igstk tracking for old config files
	}
	else if(trackingsystemNodes.count() > 1)
	{
		CX_LOG_ERROR() << "ConfigurationFileParser::getTrackingSystem(): Config file: " << mConfigurationFilePath
									 << " has more the one tracking system. Only one is currently supported.";
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

	QDomNodeList toolFileNodes = mConfigureDoc.elementsByTagName(CONFIG_TRACKER_TOOL_FILE);
	for (int i = 0; i < toolFileNodes.count(); ++i)
	{
		ToolStructure toolStructure;
		toolStructure.mAbsoluteToolFilePath = this->getAbsoluteToolFilePath(toolFileNodes.at(i).toElement());
		toolStructure.mOpenIGTLinkTransformId = toolFileNodes.at(i).toElement().attribute(OPENIGTLINK_TRANSFORM_ID_ATTRIBUTE);
		toolStructure.mOpenIGTLinkImageId = toolFileNodes.at(i).toElement().attribute(OPENIGTLINK_IMAGE_ID_ATTRIBUTE);

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
	bool doSaveFile = true;
	QDomDocument doc;
	doc.appendChild(doc.createProcessingInstruction("xml version =", "\"1.0\""));

	QDomElement configNode = doc.createElement(CONFIG_TAG);
	configNode.setAttribute(CLINICAL_APP_ATTRIBUTE, config.mClinical_app);

	QDomElement trackingsystemNode = doc.createElement(CONFIG_TRACKINGSYSTEM_TAG);
	trackingsystemNode.setAttribute(TYPE_ATTRIBUTE, config.mTrackingSystem);

	configNode.appendChild(trackingsystemNode);

	TrackersAndToolsMap::iterator it1 = config.mTrackersAndTools.begin();
	for (; it1 != config.mTrackersAndTools.end(); ++it1)
	{
		QString trackerType = enum2string(it1->first);
		if(trackerType.isEmpty())
			trackerType=config.mTrackingSystem;
		QDomElement trackerTagNode = doc.createElement(CONFIG_TRACKER_TAG);
		trackerTagNode.setAttribute(TYPE_ATTRIBUTE, trackerType);

		ToolStructureVector::iterator it2 = it1->second.begin();
		for (; it2 != it1->second.end(); ++it2)
		{
			QString absoluteToolFilePath = it2->mAbsoluteToolFilePath;
			QString relativeToolFilePath = convertToRelativeToolFilePath(config.mFileName, absoluteToolFilePath);

			ToolFileParser toolparser(absoluteToolFilePath);
			QString toolTrackerType = enum2string(toolparser.getTool()->mTrackerType);
			if (!trackerType.contains(enum2string(toolparser.getTool()->mTrackerType), Qt::CaseInsensitive))
			{
				if (config.mTrackingSystem.contains("openigtlink", Qt::CaseInsensitive))
				{
					doSaveFile = false;
				}
				else
				reportWarning("When saving configuration, skipping tool " + relativeToolFilePath + " of type "
												+ toolTrackerType + " because tracker is set to " + trackerType);
				continue;
			}

			QDomElement toolFileNode = doc.createElement(CONFIG_TRACKER_TOOL_FILE);
			toolFileNode.appendChild(doc.createTextNode(relativeToolFilePath));
			toolFileNode.setAttribute(REFERENCE_ATTRIBUTE, (it2->mReference ? "yes" : "no"));
			//TODO: Set attributes openigtlinktransformid and openigtlinkimageid
			trackerTagNode.appendChild(toolFileNode);
		}
		trackingsystemNode.appendChild(trackerTagNode);
	}

	doc.appendChild(configNode);

	//write to file
	QFile file(config.mFileName);
	if(doSaveFile)
	{
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
	else
		CX_LOG_INFO() << "Changing OpenIGTLink tool files not supported. Not overwriting config file: " << file.fileName();
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
