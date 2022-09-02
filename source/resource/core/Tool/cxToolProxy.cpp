#include "cxToolProxy.h"
#include "boost/bind.hpp"
#include "cxNullDeleter.h"
#include "cxTrackingService.h"
#include "cxTypeConversions.h"

namespace cx
{

ToolPtr ToolProxy::create(ToolPtr base, ctkPluginContext *pluginContext)
{
	return boost::shared_ptr<ToolProxy>(new ToolProxy(base, pluginContext));
}

ToolProxy::ToolProxy(ToolPtr base, ctkPluginContext *pluginContext)
{
	mTool = base;
	mPluginContext = pluginContext;
	this->initServiceListener();
}

void ToolProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<TrackingService>(
							   mPluginContext,
							   boost::bind(&ToolProxy::onServiceAdded, this, _1),
							   boost::function<void (TrackingService*)>(),
							   boost::bind(&ToolProxy::onServiceRemoved, this, _1)
							   ));
	mServiceListener->open();
}

void ToolProxy::onServiceAdded(TrackingService* service)
{
	mTrackingService.reset(service, null_deleter());

	connect(mTrackingService.get(), &TrackingService::stateChanged, this, &ToolProxy::onStateChanged);

	this->checkToolValidity();
}

void ToolProxy::onServiceRemoved(TrackingService *service)
{
	mTrackingService = TrackingService::getNullObject();
	mTool = Tool::getNullObject();
	// emit changed signals
}

void ToolProxy::onStateChanged()
{
	this->checkToolValidity();
}

void ToolProxy::checkToolValidity()
{
	if (mTool != mTrackingService->getTool(mTool->getUid()))
	{
		std::cout << "disabling tool reference for " << mTool->getUid() << ", [currently disabled]" << std::endl;
		// enable this when tool manager is a first-class plugin
		//		mTool = Tool::getNullObject();
		// emit changed signals
	}
}

std::set<Tool::Type> ToolProxy::getTypes() const
{
	return mTool->getTypes();
}

vtkPolyDataPtr ToolProxy::getGraphicsPolyData() const
{
	return mTool->getGraphicsPolyData();
}

TimedTransformMapPtr ToolProxy::getPositionHistory()
{
	return mTool->getPositionHistory();
}

ToolPositionMetadata ToolProxy::getMetadata() const
{
	return mTool->getMetadata();
}

const std::map<double, ToolPositionMetadata>& ToolProxy::getMetadataHistory()
{
	return mTool->getMetadataHistory();
}

bool ToolProxy::getVisible() const
{
	return mTool->getVisible();
}

bool ToolProxy::isInitialized() const
{
	return mTool->isInitialized();
}

QString ToolProxy::getUid() const
{
	return mTool->getUid();
}


QString ToolProxy::getName() const
{
	return mTool->getName();
}

bool ToolProxy::isCalibrated() const
{
	return mTool->isCalibrated();
}
Transform3D ToolProxy::getCalibration_sMt() const
{
	return mTool->getCalibration_sMt();
}

void ToolProxy::setCalibration_sMt(Transform3D calibration)
{
	mTool->setCalibration_sMt(calibration);
}

ProbePtr ToolProxy::getProbe() const
{
	return mTool->getProbe();
}

double ToolProxy::getTimestamp() const
{
	return mTool->getTimestamp();
}

void ToolProxy::printSelf(std::ostream &os, Indent indent)
{
	mTool->printSelf(os, indent);
}

double ToolProxy::getTooltipOffset() const
{
	return mTool->getTooltipOffset();
}

void ToolProxy::setTooltipOffset(double val)
{
	mTool->setTooltipOffset(val);
}

std::map<QString, Vector3D> ToolProxy::getReferencePoints() const
{
	return mTool->getReferencePoints();
}

bool ToolProxy::hasReferencePointWithId(QString id)
{
	return mTool->hasReferencePointWithId(id);
}

TimedTransformMap ToolProxy::getSessionHistory(double startTime, double stopTime)
{
	return mTool->getSessionHistory(startTime, stopTime);
}

Transform3D ToolProxy::get_prMt() const
{
	return mTool->get_prMt();
}

void ToolProxy::resetTrackingPositionFilter(TrackingPositionFilterPtr filter)
{
	mTool->resetTrackingPositionFilter(filter);
}

void ToolProxy::set_prMt(const Transform3D& prMt, double timestamp)
{
	mTool->set_prMt(prMt, timestamp);
}

void ToolProxy::setVisible(bool vis)
{
	mTool->setVisible(vis);
}

ToolPtr ToolProxy::getBaseTool()
{
	return mTool->getBaseTool();
}


bool ToolProxy::isNull()
{
	return mTool->isNull();
}

} // namespace cx
