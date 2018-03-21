/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#define _USE_MATH_DEFINES

#include "cxTrackingImplService.h"

#include "boost/bind.hpp"

#include <QTimer>
#include <QDir>
#include <QList>
#include <QMetaType>
#include <QFileInfo>
#include <vtkDoubleArray.h>
#include <QCoreApplication>

#include "cxRegistrationTransform.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxPositionStorageFile.h"
#include "cxTime.h"
#include "cxEnumConverter.h"
#include "cxDummyTool.h"
#include "cxToolImpl.h"
#include "cxToolConfigurationParser.h"
#include "cxManualToolAdapter.h"
#include "cxSettings.h"
#include "cxPlaybackTool.h"

#include "cxPlaybackTime.h"
#include "cxTrackingPositionFilter.h"
#include "cxXMLNodeWrapper.h"
#include "cxTrackerConfigurationImpl.h"
#include "cxUtilHelpers.h"

#include "cxTrackingSystemDummyService.h"
#include "cxTrackingSystemPlaybackService.h"
#include "cxSessionStorageServiceProxy.h"

#include "cxNullDeleter.h"

namespace cx
{

TrackingImplService::TrackingImplService(ctkPluginContext *context) :
				mLastLoadPositionHistory(0),
				mContext(context),
				mToolTipOffset(0)
{
	mSession = SessionStorageServiceProxy::create(mContext);
	connect(mSession.get(), &SessionStorageService::sessionChanged, this, &TrackingImplService::onSessionChanged);
	connect(mSession.get(), &SessionStorageService::cleared, this, &TrackingImplService::onSessionCleared);
	connect(mSession.get(), &SessionStorageService::isLoading, this, &TrackingImplService::onSessionLoad);
	connect(mSession.get(), &SessionStorageService::isSaving, this, &TrackingImplService::onSessionSave);

	this->initializeManualTool(); // do this after setting self.

	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));

    this->listenForTrackingSystemServices(context);
}

TrackingImplService::~TrackingImplService()
{
	while (!mTrackingSystems.empty())
		this->unInstallTrackingSystem(mTrackingSystems.back());
}


bool TrackingImplService::isNull()
{
	return false;
}

void TrackingImplService::onSystemStateChanged()
{
	this->rebuildCachedTools();
	emit stateChanged();
}

/**Set playback mode.
 * Set endpoints into controller.
 *
 * The original tools are wrapped by playback tools. The original ones are
 * not changed, only their movement is ignored.
 */
void TrackingImplService::setPlaybackMode(PlaybackTimePtr controller)
{
	// playback off, reinstall old tracking systems
	if (mPlaybackSystem)
	{
		mPlaybackSystem->setState(Tool::tsNONE);
		this->unInstallTrackingSystem(mPlaybackSystem);
		std::vector<TrackingSystemServicePtr> old = mPlaybackSystem->getBase();
		for (unsigned i=0; i<old.size(); ++i)
			this->installTrackingSystem(old[i]);
		mPlaybackSystem.reset();
	}

	// uninstall tracking systems, playback on
	if (controller)
	{
		mPlaybackSystem.reset(new TrackingSystemPlaybackService(controller, mTrackingSystems, mManualTool));
		std::vector<TrackingSystemServicePtr> old = mPlaybackSystem->getBase();
		for (unsigned i=0; i<old.size(); ++i)
			this->unInstallTrackingSystem(old[i]);
		this->installTrackingSystem(mPlaybackSystem);
		mPlaybackSystem->setState(Tool::tsTRACKING);
	}
}

bool TrackingImplService::isPlaybackMode() const
{
	return mPlaybackSystem && (mPlaybackSystem->getState()>=Tool::tsCONFIGURED);
}

void TrackingImplService::runDummyTool(DummyToolPtr tool)
{
	TrackingSystemServicePtr dummySystem;
	dummySystem.reset(new TrackingSystemDummyService(tool));
	this->installTrackingSystem(dummySystem);

	dummySystem->setState(Tool::tsTRACKING);
	this->setActiveTool(tool->getUid());
}

void TrackingImplService::installTrackingSystem(TrackingSystemServicePtr system)
{
	mTrackingSystems.push_back(system);
	report("Installing tracking system: " + system->getUid());
	connect(system.get(), &TrackingSystemService::stateChanged, this, &TrackingImplService::onSystemStateChanged);
	this->onSystemStateChanged();
}

void TrackingImplService::unInstallTrackingSystem(TrackingSystemServicePtr system)
{
	report("Uninstalling tracking system: " + system->getUid());
	disconnect(system.get(), &TrackingSystemService::stateChanged, this, &TrackingImplService::onSystemStateChanged);

	for (unsigned i=0; i<mTrackingSystems.size(); ++i)
	{
		if (mTrackingSystems[i]!=system)
			continue;
		mTrackingSystems.erase(mTrackingSystems.begin()+i);
		break;
	}

	this->onSystemStateChanged();
}

std::vector<TrackingSystemServicePtr> TrackingImplService::getTrackingSystems()
{
	return mTrackingSystems;
}

void TrackingImplService::initializeManualTool()
{
	if (!mManualTool)
	{
		//adding a manual tool as default
		mManualTool.reset(new ManualToolAdapter("ManualTool"));
		mTools["ManualTool"] = mManualTool;
		mManualTool->setVisible(true);
		connect(mManualTool.get(), &Tool::toolVisible, this, &TrackingImplService::activeCheckSlot);
		connect(mManualTool.get(), &Tool::toolTransformAndTimestamp, this, &TrackingImplService::activeCheckSlot);
		connect(mManualTool.get(), &Tool::tooltipOffset, this, &TrackingImplService::onTooltipOffset);
	}

	Transform3D rMpr = Transform3D::Identity(); // not known: not really important either
	Transform3D prMt = rMpr.inv() * createTransformRotateY(M_PI) * createTransformRotateZ(M_PI/2);
	mManualTool->set_prMt(prMt);
	this->activeCheckSlot();
}

Tool::State TrackingImplService::getState() const
{
	Tool::State state = Tool::tsNONE;
	for (unsigned i=0; i<mTrackingSystems.size(); ++i)
		state = std::max(state, mTrackingSystems[i]->getState());
	return state;
}

void TrackingImplService::setState(const Tool::State val)
{
	for (unsigned i=0; i<mTrackingSystems.size(); ++i)
		mTrackingSystems[i]->setState(val);
}

void TrackingImplService::listenForTrackingSystemServices(ctkPluginContext *context)
{
    mServiceListener.reset(new ServiceTrackerListener<TrackingSystemService>(
                                context,
                                boost::bind(&TrackingImplService::onTrackingSystemAdded, this, _1),
                                boost::bind(&TrackingImplService::onTrackingSystemModified, this, _1),
                                boost::bind(&TrackingImplService::onTrackingSystemRemoved, this, _1)
                               ));
    mServiceListener->open();
}

void TrackingImplService::onTrackingSystemAdded(TrackingSystemService* service)
{
    this->installTrackingSystem(TrackingSystemServicePtr(service, null_deleter()));
}

void TrackingImplService::onTrackingSystemRemoved(TrackingSystemService* service)
{
    this->unInstallTrackingSystem(TrackingSystemServicePtr(service, null_deleter()));
}

void TrackingImplService::onTrackingSystemModified(TrackingSystemService* service)
{
	Q_UNUSED(service);
}

void TrackingImplService::rebuildCachedTools()
{
    mTools.clear();
    for (unsigned i=0; i<mTrackingSystems.size(); ++i)
    {
        this->addToolsFrom(mTrackingSystems[i]);
    }
    mTools[mManualTool->getUid()] = mManualTool;
    this->imbueManualToolWithRealProperties();
    this->loadPositionHistory(); // the tools are always reconfigured after a setloggingfolder
	this->resetTrackingPositionFilters();
	this->onTooltipOffset(mToolTipOffset);
	this->setActiveTool(this->getManualTool()->getUid()); // this emits a signal: call after all other initialization
}

void TrackingImplService::imbueManualToolWithRealProperties()
{
	// debug: give the manual tool properties from the first non-manual tool. Nice for testing tools
	if (!settings()->value("giveManualToolPhysicalProperties").toBool())
		return;

	for (ToolMap::iterator iter = mTools.begin(); iter != mTools.end(); ++iter)
	{
		if (iter->second == mManualTool)
			continue;
		if (iter->second->hasType(Tool::TOOL_REFERENCE))
			continue;
		mManualTool->setBase(iter->second);
		mManualTool->startEmittingContinuousPositions(100);

		report("Manual tool imbued with properties from " + iter->first);
		break;
	}
}

void TrackingImplService::addToolsFrom(TrackingSystemServicePtr system)
{
	std::vector<ToolPtr> tools = system->getTools();
	for (unsigned i=0; i<tools.size(); ++i)
	{
		ToolPtr tool = tools[i];
		mTools[tool->getUid()] = tool;
		connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(activeCheckSlot()));
		connect(tool.get(), &Tool::toolTransformAndTimestamp, this, &TrackingImplService::activeCheckSlot);
		connect(tool.get(), &Tool::tooltipOffset, this, &TrackingImplService::onTooltipOffset);

		if (tool->hasType(Tool::TOOL_REFERENCE))
			mReferenceTool = tool;
	}
}

void TrackingImplService::onTooltipOffset(double val)
{
	mToolTipOffset = val;
	for (ToolMap::iterator iter = mTools.begin(); iter != mTools.end(); ++iter)
	{
		iter->second->setTooltipOffset(val);
	}
}

SessionToolHistoryMap TrackingImplService::getSessionHistory(double startTime, double stopTime)
{
	SessionToolHistoryMap retval;

	ToolMap tools = this->getTools();
	ToolMap::iterator it = tools.begin();
	for (; it != tools.end(); ++it)
	{
		TimedTransformMap toolMap = it->second->getSessionHistory(startTime, stopTime);
		if (toolMap.empty())
			continue;
		retval[it->second] = toolMap;
	}
	return retval;
}

ToolMap TrackingImplService::getTools()
{
	return mTools;
}

ToolPtr TrackingImplService::getTool(const QString& uid)
{
	if (uid == "active")
		return this->getActiveTool();

	ToolPtr retval;
	ToolMap::iterator it = mTools.find(uid);
	if (it != mTools.end())
		retval = it->second;

	return retval;
}

ToolPtr TrackingImplService::getActiveTool()
{
	return mActiveTool;
}

void TrackingImplService::setActiveTool(const QString& uid)
{
	if (mActiveTool && mActiveTool->getUid() == uid)
		return;

	ToolPtr newTool;
	newTool = this->getTool(uid);

	ToolPtr oldTool = mActiveTool;
	mActiveTool = newTool; // set active before calling setters, which possibly can emit signal and cause cycles.

	// special case for manual tool
	if (newTool && newTool->hasType(Tool::TOOL_MANUAL) && mManualTool)
	{
		if (oldTool && (oldTool!=mManualTool))
			mManualTool->set_prMt(oldTool->get_prMt(), oldTool->getTimestamp() -1);
		mManualTool->setVisible(true);
	}
	else
	{
		mManualTool->setVisible(false);
	}

	emit activeToolChanged(uid);
}

ToolPtr TrackingImplService::getReferenceTool() const
{
	return mReferenceTool;
}

void TrackingImplService::savePositionHistory()
{
	QString filename = this->getLoggingFolder() + "/toolpositions.snwpos";

	PositionStorageWriter writer(filename);

	ToolMap::iterator it = mTools.begin();
	for (; it != mTools.end(); ++it)
	{
		ToolPtr current = it->second;
		TimedTransformMapPtr data = current->getPositionHistory();

		if (!data)
			continue;

		// save only data acquired after mLastLoadPositionHistory:
		TimedTransformMap::iterator iter = data->lower_bound(mLastLoadPositionHistory);
		for (; iter != data->end(); ++iter)
			writer.write(iter->second, (iter->first), current->getUid());
	}

	mLastLoadPositionHistory = getMilliSecondsSinceEpoch();
}

void TrackingImplService::loadPositionHistory()
{
	if (this->getState()==Tool::tsNONE)
		return;
	// save all position data acquired so far, in case of multiple calls.
	this->savePositionHistory();

	QString filename = this->getLoggingFolder()+ "/toolpositions.snwpos";

	PositionStorageReader reader(filename);

	Transform3D matrix = Transform3D::Identity();
	double timestamp;
	QString toolUid;

	QStringList missingTools;

	while (!reader.atEnd())
	{
		if (!reader.read(&matrix, &timestamp, &toolUid))
			break;

		ToolPtr current = this->getTool(toolUid);
		if (current)
		{
			(*current->getPositionHistory())[timestamp] = matrix;
		}
		else
		{
			missingTools << toolUid;
		}
	}

	missingTools.removeDuplicates();
	missingTools.removeAll("");

	if (!missingTools.empty())
	{
		reportWarning(QString("Loaded position history, but some of the tools "
							  "are not present in the configuration:"
							  "\n  \t%1").arg(missingTools.join("\n  \t")));
	}

	mLastLoadPositionHistory = getMilliSecondsSinceEpoch();
}

//void TrackingImplService::setLoggingFolder(QString loggingFolder)
//{
//	if (mLoggingFolder == loggingFolder)
//		return;

//	for (unsigned i=0; i<mTrackingSystems.size(); ++i)
//		mTrackingSystems[i]->setLoggingFolder(loggingFolder);
//	mLoggingFolder = loggingFolder;
//}

void TrackingImplService::globalConfigurationFileChangedSlot(QString key)
{
	if (key.contains("TrackingPositionFilter"))
	{
		this->resetTrackingPositionFilters();
	}
}

void TrackingImplService::resetTrackingPositionFilters()
{
	bool enabled = settings()->value("TrackingPositionFilter/enabled", false).toBool();
	double cutoff = settings()->value("TrackingPositionFilter/cutoffFrequency", 0).toDouble();

	for (ToolMap::iterator iter=mTools.begin(); iter!=mTools.end(); ++iter)
	{
		TrackingPositionFilterPtr filter;
		if (enabled)
		{
			filter.reset(new TrackingPositionFilter());
			if (cutoff>0.01)
				filter->setCutOffFrequency(cutoff);
		}
		iter->second->resetTrackingPositionFilter(filter);
	}
}

void TrackingImplService::activeCheckSlot()
{
	if (this->manualToolHasMostRecentTimestamp() && mManualTool->getVisible())
	{
		this->setActiveTool(this->getManualTool()->getUid());
		return;
	}

	bool use = settings()->value("Automation/autoSelectActiveTool").toBool();
	if (!use)
		return;

	//make a sorted vector of all visible tools
	std::vector<ToolPtr> tools = this->getVisibleTools();
	tools.push_back(mManualTool);

	if (!tools.empty())
	{
		//sort most important tool to the start of the vector:
		sort(tools.begin(), tools.end(), toolTypeSort);
		const QString uid = tools[0]->getUid();
		this->setActiveTool(uid);
	}
}

bool TrackingImplService::manualToolHasMostRecentTimestamp()
{
	// original comment (was wrapped in an ifplayblack):
	// In static playback mode, tools does not turn invisible since
	// time dont move. Here we check whether manual tool has a newer
	// timestamp than the playback tools. If it has, make it active.
	// This enables automatic change to manual tool if the user
	// manipulates the manual tool in some way.

	double mts = this->getManualTool()->getTimestamp();
//	std::cout << "  manual tooltime " << mts << std::endl;

	double bestTime = 0;
	for (ToolMap::iterator it = mTools.begin(); it != mTools.end(); ++it)
	{
//		std::cout << "  tool " << it->first << " -- "<< it->second->getTimestamp() << std::endl;
		if (it->second->hasType(Tool::TOOL_MANUAL))
			continue;
//		std::cout << "    tool " << it->first << " : "<< it->second->getTimestamp() - mts << std::endl;
		bestTime = std::max(bestTime, it->second->getTimestamp());
	}
//	double ahead = mts -bestTime;
//	std::cout << "  mts -bestTime " << " : "<< (mts -bestTime) << std::endl;
//	std::cout << "  mts > bestTime " << " : "<< bool(mts > bestTime) << std::endl;

	return (mts > bestTime);
}

std::vector<ToolPtr> TrackingImplService::getVisibleTools()
{
	std::vector<ToolPtr> retval;
	for (ToolMap::iterator it = mTools.begin(); it != mTools.end(); ++it)
		if (it->second->getVisible())
			retval.push_back(it->second);
	return retval;
}



namespace
{
/**
 * \return a priority of the tool. High means this tool is preferred more.
 */
int getPriority(ToolPtr tool)
{
	if (tool->hasType(Tool::TOOL_MANUAL)) // place this first, in case a tool has several attributes.
		return 2;

	if (tool->hasType(Tool::TOOL_US_PROBE))
		return 4;
	if (tool->hasType(Tool::TOOL_POINTER))
		return 3;
	if (tool->hasType(Tool::TOOL_REFERENCE))
		return 1;
	return 0;
}
}

/**
 * sorts tools in descending order of type
 * @param tool1 the first tool
 * @param tool2 the second tool
 * @return whether the second tool is of higher priority than the first or not
 */
bool toolTypeSort(const ToolPtr tool1, const ToolPtr tool2)
{
	return getPriority(tool2) < getPriority(tool1);
}

QString TrackingImplService::getLoggingFolder()
{
	return mSession->getSubFolder("Logs/");
}

void TrackingImplService::onSessionChanged()
{
	QString loggingFolder = this->getLoggingFolder();

	for (unsigned i=0; i<mTrackingSystems.size(); ++i)
		mTrackingSystems[i]->setLoggingFolder(loggingFolder);
}

void TrackingImplService::onSessionCleared()
{
	mManualTool->set_prMt(Transform3D::Identity());
}

void TrackingImplService::onSessionLoad(QDomElement& node)
{
	XMLNodeParser root(node);
	QDomElement toolManagerNode = root.descend("managers/toolManager").node().toElement();
	if (!toolManagerNode.isNull())
		this->parseXml(toolManagerNode);

}

void TrackingImplService::onSessionSave(QDomElement& node)
{
	XMLNodeAdder root(node);
	QDomElement managerNode = root.descend("managers").node().toElement();
	this->addXml(managerNode);

	this->savePositionHistory();
}

void TrackingImplService::addXml(QDomNode& parentNode)
{
	XMLNodeAdder parent(parentNode);
	XMLNodeAdder base(parent.addElement("toolManager"));

	base.addTextToElement("toolTipOffset", qstring_cast(mToolTipOffset));
	base.addTextToElement("manualTool", "\n" + qstring_cast(mManualTool->get_prMt()));

	//Tools
	XMLNodeAdder toolsNode(base.addElement("tools"));
	ToolMap tools = this->getTools();
	ToolMap::iterator toolIt = tools.begin();
	for (; toolIt != tools.end(); ++toolIt)
	{
        cxToolPtr tool = boost::dynamic_pointer_cast<ToolImpl>(toolIt->second);
		if (tool)
		{
			toolsNode.addObjectToElement("tool", tool);
		}
	}

}

void TrackingImplService::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;

	XMLNodeParser base(dataNode);

	QString manualToolText = dataNode.namedItem("manualTool").toElement().text();
	mManualTool->set_prMt(Transform3D::fromString(manualToolText));

	mToolTipOffset = base.parseDoubleFromElementWithDefault("toolTipOffset", 0.0);
	this->onTooltipOffset(mToolTipOffset);

	//Tools
	ToolMap tools = this->getTools();
	XMLNodeParser toolssNode(dataNode.namedItem("tools"));
	std::vector<QDomElement> toolNodes = toolssNode.getDuplicateElements("tool");

	for (unsigned i=0; i<toolNodes.size(); ++i)
	{
		QDomElement toolNode = toolNodes[i];
		QString tool_uid = toolNode.attribute("uid");
		if (tools.find(tool_uid) != tools.end())
		{
            cxToolPtr tool = boost::dynamic_pointer_cast<ToolImpl>(tools.find(tool_uid)->second);
			tool->parseXml(toolNode);
		}
	}
}

ToolPtr TrackingImplService::getManualTool()
{
	return mManualTool;
}

/**\brief Find a probe that can be connected to a rt source.
 *
 * Priority:
 *  - active probe
 *  - visible probe
 *  - any probe
 *
 */
ToolPtr TrackingImplService::getFirstProbe()
{
	ToolPtr active = this->getActiveTool();

	// Turned off the check for valid probe fix #1038: No probe sector in 3D scene if no xml emtry for probe.
	// This will work for digital interfaces where the probe sector definition comes from the interface, and not from the xml file.
//	if (active && active->getProbe() && active->getProbe()->isValid())
	if (active && active->getProbe())
		return active;

	ToolMap tools = this->getTools();

	// look for visible probes
	for (ToolMap::iterator iter = tools.begin(); iter != tools.end(); ++iter)
		if (iter->second->getProbe() && iter->second->getProbe()->isValid() && iter->second->getVisible())
			return iter->second;

	// pick the first probe, visible or not.
	for (ToolMap::iterator iter = tools.begin(); iter != tools.end(); ++iter)
		if (iter->second->getProbe() && iter->second->getProbe()->isValid())
			return iter->second;

	return ToolPtr();
}

//This may not work if more than one tracking system returns a configuration?
TrackerConfigurationPtr TrackingImplService::getConfiguration()
{
	TrackerConfigurationPtr config;
	for (unsigned i=0; i<mTrackingSystems.size(); ++i)
	{
		TrackerConfigurationPtr config2 = mTrackingSystems[i]->getConfiguration();
		if (config2)
		{
			config = config2;
//			CX_LOG_DEBUG() << "getConfiguration config TrackingSystemSolution: " << config->getTrackingSystemImplementation();
		}
	}
	return config;
}


} //namespace cx
