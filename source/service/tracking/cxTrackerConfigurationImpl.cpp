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

#include "cxTrackerConfigurationImpl.h"
#include "cxDataLocations.h"
#include "cxReporter.h"
#include "cxDefinitionStrings.h"
#include "cxToolConfigurationParser.h"
#include "cxFileHelpers.h"

namespace cx
{

void TrackerConfigurationImpl::saveConfiguration(const Configuration& config)
{
	ConfigurationFileParser::Configuration data;
	data.mFileName = config.mFileName;
	data.mClinical_app = string2enum<CLINICAL_APPLICATION>(config.mClinicalApplication);
//    retval.mClinicalApplication = mApplicationGroupBox->getSelected()[0];
//    retval.mTrackingSystem = mTrackingSystemGroupBox->getSelected()[0];
//    retval.mTools = mToolListWidget->getTools();
//    retval.mReferenceTool = mReferenceComboBox->itemData(mReferenceComboBox->currentIndex(), Qt::ToolTipRole).toString();

	QStringList selectedTools = config.mTools;
	QString referencePath = config.mReferenceTool;

	TRACKING_SYSTEM selectedTracker = string2enum<TRACKING_SYSTEM>(config.mTrackingSystem);

	ConfigurationFileParser::ToolFilesAndReferenceVector toolfilesAndRefVector;
	QFile configFile(data.mFileName);
	QFileInfo info(configFile);
	QDir dir = info.dir();
	foreach(QString absoluteToolPath, selectedTools)
	{
	  QString relativeToolFilePath = dir.relativeFilePath(absoluteToolPath);
  //    std::cout << "Relative tool file path: " << relativeToolFilePath << std::endl;

	  ConfigurationFileParser::ToolFileAndReference tool;
	  tool.first = relativeToolFilePath;

  //    std::cout << "====" << std::endl;
  //    std::cout << "absoluteToolPath " << absoluteToolPath << std::endl;
  //    std::cout << "referencePath " << referencePath << std::endl;
	  tool.second = (absoluteToolPath == referencePath);
	  toolfilesAndRefVector.push_back(tool);
	}

	data.mTrackersAndTools[selectedTracker] = toolfilesAndRefVector;

	ConfigurationFileParser::saveConfiguration(data);
}

TrackerConfigurationImpl::Configuration TrackerConfigurationImpl::getConfiguration(QString uid)
{
	ConfigurationFileParser parser(uid);

	Configuration retval;
	retval.mFileName = uid;

	CLINICAL_APPLICATION application = parser.getApplicationapplication();
	retval.mClinicalApplication = enum2string(application);

	std::vector<IgstkTracker::InternalStructure> trackers = parser.getTrackers();
	for (unsigned i = 0; i < trackers.size(); ++i)
	{
		retval.mTrackingSystem = enum2string(trackers[i].mType);
		// only one trackingsystem is returned. (backed supports more than is needed.)
	}

	std::vector<QString> tools = parser.getAbsoluteToolFilePaths();
	for (unsigned i = 0; i < tools.size(); ++i)
	{
		retval.mTools << tools[i];
	}

	retval.mReferenceTool = parser.getAbsoluteReferenceFilePath();

	return retval;
}

QStringList TrackerConfigurationImpl::getToolsGivenFilter(QStringList applicationsFilter,
														  QStringList trackingsystemsFilter)
{
	QStringList allTools = this->getAbsoluteFilePathToAllTools();
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
	IgstkTool::InternalStructure internal = parser.getTool();

	retval.mTrackingSystem = enum2string(internal.mTrackerType);
	retval.mIsReference = internal.mIsReference;
	retval.mPictureFilename = internal.mPictureFileName;

	return retval;
}

QString TrackerConfigurationImpl::getToolName(QString uid)
{
	QString absoluteFilePath = uid;
	QFile file(absoluteFilePath);
	QFileInfo info(file);
	return info.dir().dirName();
}

QString TrackerConfigurationImpl::getToolTrackingSystem(QString uid)
{
	QString absoluteFilePath = uid;
	ToolFileParser parser(absoluteFilePath);
	QString toolTrackingSystem = enum2string(parser.getTool().mTrackerType);
	return toolTrackingSystem;
}

QString TrackerConfigurationImpl::getToolPictureFilename(QString uid)
{
	QString absoluteFilePath = uid;
	IgstkTool::InternalStructure tool;

	ToolFileParser parser(absoluteFilePath);
	tool = parser.getTool();

	return tool.mPictureFileName;
}

QStringList TrackerConfigurationImpl::filter(QStringList toolsToFilter, QStringList applicationsFilter,
		QStringList trackingsystemsFilter)
{
	QStringList retval;
//	TrackerConfigurationPtr config = toolManager()->getConfiguration();
//	std::vector<ToolConfigurationPtr> tools = config->getTools();

	foreach(QString toolFilePath, toolsToFilter)
	{
		//get internal tool
		IgstkTool::InternalStructure internal = this->getToolInternal(toolFilePath);

		//check tracking systems
		QString trackerName = enum2string(internal.mTrackerType);
		if(!trackingsystemsFilter.contains(trackerName, Qt::CaseInsensitive))
		continue;

		//check applications
		bool passedApplicationFilter = false;
		std::vector<CLINICAL_APPLICATION>::iterator it = internal.mClinicalApplications.begin();
		while(it != internal.mClinicalApplications.end() && !passedApplicationFilter)
		{
			QString applicationName = enum2string(*it);
			if(applicationsFilter.contains(applicationName, Qt::CaseInsensitive))
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

IgstkTool::InternalStructure TrackerConfigurationImpl::getToolInternal(QString toolAbsoluteFilePath)
{
	IgstkTool::InternalStructure retval;

	ToolFileParser parser(toolAbsoluteFilePath);
	retval = parser.getTool();

	return retval;
}

QStringList TrackerConfigurationImpl::getAbsoluteFilePathToAllTools()
{
	bool includeSubDirs = true;
	QString toolFilePath = cx::DataLocations::getToolsPath();
	return getAbsolutePathToXmlFiles(toolFilePath, includeSubDirs);
}

bool TrackerConfigurationImpl::verifyTool(QString uid)
{
	IgstkTool::InternalStructure internal = this->getToolInternal(uid);
	return internal.verify();
}

} // namespace cx

