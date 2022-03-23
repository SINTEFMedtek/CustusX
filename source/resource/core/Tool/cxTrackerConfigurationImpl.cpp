/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackerConfigurationImpl.h"
#include "cxDataLocations.h"

#include "cxToolConfigurationParser.h"
#include "cxFileHelpers.h"
#include "cxProfile.h"
#include "cxTracker.h"
#include "cxLogger.h"
#include "cxEnumConversion.h"

namespace cx
{

void TrackerConfigurationImpl::saveConfiguration(const Configuration& config)
{
	ConfigurationFileParser::Configuration data;
	data.mFileName = config.mUid;
	data.mClinical_app = config.mClinicalApplication;
	data.mTrackingSystemImplementation = config.mTrackingSystemImplementation;
	data.mApplyRefToTools = config.mApplyRefToTools;

	QStringList selectedTools = config.mTools;
	QString referencePath = config.mReferenceTool;

	TRACKING_SYSTEM selectedTracker = string2enum<TRACKING_SYSTEM>(config.mTrackingSystemName);

	ConfigurationFileParser::ToolStructureVector toolStructureVector;
//	QFile configFile(data.mFileName);
//	QFileInfo info(configFile);
//	QDir dir = info.dir();
	foreach(QString absoluteToolPath, selectedTools)
	{
//	  QString relativeToolFilePath = dir.relativeFilePath(absoluteToolPath);
		ConfigurationFileParser::ToolStructure tool;
		tool.mAbsoluteToolFilePath = absoluteToolPath;
		tool.mReference = (absoluteToolPath == referencePath);

		for (unsigned i = 0; i < config.mToolList.size(); ++i)
		{
			if(tool.mAbsoluteToolFilePath == config.mToolList[i].mAbsoluteToolFilePath)
			{
				tool.mOpenIGTLinkImageId = config.mToolList[i].mOpenIGTLinkImageId;
				tool.mOpenIGTLinkTransformId = config.mToolList[i].mOpenIGTLinkTransformId;
			}
		}

		toolStructureVector.push_back(tool);
	}

	data.mTrackersAndTools[selectedTracker] = toolStructureVector;

	ConfigurationFileParser::saveConfiguration(data);
}

TrackerConfiguration::Configuration TrackerConfigurationImpl::getConfiguration(QString uid)
{
	ConfigurationFileParser parser(uid);

	Configuration retval;
	retval.mUid = uid;
	retval.mName = QFileInfo(uid).completeBaseName();

	retval.mClinicalApplication = parser.getApplicationapplication();

	std::vector<ToolFileParser::TrackerInternalStructure> trackers = parser.getTrackers();
	for (unsigned i = 0; i < trackers.size(); ++i)
	{
		retval.mTrackingSystemName = enum2string(trackers[i].mType);
		// only one trackingsystem is returned. (backed supports more than is needed.)
	}

	std::vector<QString> tools = parser.getAbsoluteToolFilePaths();
	for (unsigned i = 0; i < tools.size(); ++i)
	{
		retval.mTools << tools[i];
	}

	retval.mReferenceTool = parser.getAbsoluteReferenceFilePath();
	retval.mTrackingSystemImplementation = parser.getTrackingSystemImplementation();
	retval.mApplyRefToTools = parser.getApplyRefToTools();
	retval.mToolList = parser.getToolListWithMetaInformation();

	return retval;
}

QStringList TrackerConfigurationImpl::getToolsGivenFilter(QStringList applicationsFilter,
														  QStringList trackingsystemsFilter)
{
	QStringList allTools = this->getAllTools();
	QStringList filteredTools = this->filter(allTools, applicationsFilter, trackingsystemsFilter);
	return filteredTools;
}

TrackerConfigurationImpl::Tool TrackerConfigurationImpl::getTool(QString uid)
{
	Tool retval;
	retval.mUid = uid;

	QString absoluteFilePath = uid;
	QFile file(absoluteFilePath);
	QFileInfo info(file);
	retval.mName = info.dir().dirName();

	ToolFileParser parser(absoluteFilePath);
	ToolFileParser::ToolInternalStructurePtr internal = parser.getTool();

	retval.mTrackingSystemName = enum2string(internal->mTrackerType);
	retval.mIsReference = internal->mIsReference;
	retval.mPictureFilename = internal->mPictureFileName;
	retval.mPortNumber = internal->mPortNumber;

	return retval;
}

QStringList TrackerConfigurationImpl::getAllApplications()
{
	QStringList allTools = this->getAllTools();
	QStringList retval;

	foreach(QString path, allTools)
	{
		//get internal tool
				ToolFileParser::ToolInternalStructurePtr internal = this->getToolInternal(path);
		for (unsigned i=0; i<internal->mClinicalApplications.size(); ++i)
			retval << internal->mClinicalApplications[i];
	}

	retval.removeDuplicates();
	return retval;
}

QString TrackerConfigurationImpl::getTrackingSystemImplementation()
{
	return mTrackingSystemImplementation;
}

void TrackerConfigurationImpl::setTrackingSystemImplementation(QString trackingSystemImplementation)
{
	mTrackingSystemImplementation = trackingSystemImplementation;
}

QStringList TrackerConfigurationImpl::filter(QStringList toolsToFilter, QStringList applicationsFilter,
		QStringList trackingsystemsFilter)
{
	QStringList retval;

	foreach(QString toolFilePath, toolsToFilter)
	{
		//get internal tool
				ToolFileParser::ToolInternalStructurePtr internal = this->getToolInternal(toolFilePath);

		//check tracking systems
		QString trackerName = enum2string(internal->mTrackerType);
		if(!trackingsystemsFilter.contains(trackerName, Qt::CaseInsensitive))
		continue;

		//check applications
		bool passedApplicationFilter = false;
		std::vector<QString>::iterator it = internal->mClinicalApplications.begin();
		while(it != internal->mClinicalApplications.end() && !passedApplicationFilter)
		{
			QString applicationName = *it;
			if(applicationsFilter.contains(applicationName, Qt::CaseInsensitive))
			{
				passedApplicationFilter = true;
			}
            if(applicationsFilter.contains("all", Qt::CaseInsensitive))
            {
                passedApplicationFilter = true;
            }
            if(applicationsFilter.contains("default", Qt::CaseInsensitive))
            {
                passedApplicationFilter = true;
            }
			++it;
		}
		if(!passedApplicationFilter)
		continue;

		//add if filters passed
		retval << toolFilePath;
	}

	return retval;
}

ToolFileParser::ToolInternalStructurePtr TrackerConfigurationImpl::getToolInternal(QString toolAbsoluteFilePath)
{
	ToolFileParser::ToolInternalStructurePtr retval;

	ToolFileParser parser(toolAbsoluteFilePath);
	retval = parser.getTool();

	return retval;
}

bool TrackerConfigurationImpl::verifyTool(QString uid)
{
	ToolFileParser::ToolInternalStructurePtr internal = this->getToolInternal(uid);
	return internal->verify();
}

QString TrackerConfigurationImpl::getConfigurationApplicationsPath()
{
	return profile()->getPath() + "/tool";
}

QStringList TrackerConfigurationImpl::getConfigurationsGivenApplication()
{
	QString path = this->getConfigurationApplicationsPath();
	return cx::getAbsolutePathToXmlFiles(path);

//	        QStringList retval;

//	QStringList configPaths = DataLocations::getRootConfigPaths();

//	for (int i=0; i< configPaths.size(); ++i)
//	{
//		QDir dir(configPaths[i]+"/tool/"+application);
//		retval << cx::getAbsolutePathToXmlFiles(dir.absolutePath());
//	}
//	return retval;
}

QStringList TrackerConfigurationImpl::getAllConfigurations()
{
	QStringList retval;
	QStringList rootPaths = DataLocations::getRootConfigPaths();

	for (int i=0; i< rootPaths.size(); ++i)
	{
		QString configFilePath = rootPaths[i] + "/profiles";
		foreach(QFileInfo dir, cx::getDirs(configFilePath))
		{
			retval << cx::getAbsolutePathToXmlFiles(dir.absoluteFilePath()+"/tool");
		}
	}
	return retval;
}

QStringList TrackerConfigurationImpl::getAllTools()
{
	QStringList root = profile()->getAllRootConfigPaths();
	QString suffix("/tool/Tools/");
	QStringList retval;
	bool includeSubDirs = true;
	for (int i=0; i<root.size(); ++i)
	{
		QString toolPath = root[i]+suffix;
		retval << getAbsolutePathToXmlFiles(toolPath, includeSubDirs);
	}
	return retval;
}

QStringList TrackerConfigurationImpl::getSupportedTrackingSystems()
{
	QStringList retval;
    retval = Tracker::getSupportedTrackingSystems();
	return retval;
}


} // namespace cx

