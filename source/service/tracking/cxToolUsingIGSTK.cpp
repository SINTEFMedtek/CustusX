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

#define _USE_MATH_DEFINES
#include "cxToolUsingIGSTK.h"

#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkSTLReader.h>
#include <QDir>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include "cxReporter.h"
#include "cxTypeConversions.h"
#include "cxProbeData.h"
#include "cxToolManager.h"
#include "cxProbeImpl.h"
#include "cxIgstkTool.h"
#include "cxTrackingPositionFilter.h"

namespace cx
{

ToolUsingIGSTK::ToolUsingIGSTK(TrackingServicePtr manager, IgstkToolPtr igstkTool) :
	ToolImpl(manager, ""),
				mTool(igstkTool), mPolyData(NULL),
				mValid(false), mConfigured(false), mTracked(false)
{
	Tool::mUid = mTool->getInternalStructure().mUid;
	Tool::mName = mTool->getInternalStructure().mName;
	mValid = igstkTool->isValid();

	this->createPolyData();

	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
					SLOT(toolTransformAndTimestampSlot(Transform3D, double)));
	connect(mTool.get(), SIGNAL(attachedToTracker(bool)), this, SIGNAL(attachedToTracker(bool)));
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
	connect(&mTpsTimer, SIGNAL(timeout()), this, SLOT(calculateTpsSlot()));

	if (mTool->getInternalStructure().mIsProbe)
	{
		mProbe = ProbeImpl::New(mTool->getInternalStructure().mInstrumentId,
						mTool->getInternalStructure().mInstrumentScannerId);
		connect(mProbe.get(), SIGNAL(sectorChanged()), this, SIGNAL(toolProbeSector()));
	}
	connect(this->getTrackingService().get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
}

ToolUsingIGSTK::~ToolUsingIGSTK()
{
}

std::set<ToolUsingIGSTK::Type> ToolUsingIGSTK::getTypes() const
{
	std::set<Type> retval;

	if (mTool->getInternalStructure().mIsReference)
		retval.insert(ToolUsingIGSTK::TOOL_REFERENCE);
	if (mTool->getInternalStructure().mIsPointer)
		retval.insert(ToolUsingIGSTK::TOOL_POINTER);
	if (mTool->getInternalStructure().mIsProbe)
		retval.insert(ToolUsingIGSTK::TOOL_US_PROBE);

	return retval;
}

vtkPolyDataPtr ToolUsingIGSTK::getGraphicsPolyData() const
{
	return mPolyData;
}

ProbePtr ToolUsingIGSTK::getProbe() const
{
	return mProbe;
}

bool ToolUsingIGSTK::getVisible() const
{
	return mTool->isVisible();
}

bool ToolUsingIGSTK::isInitialized() const
{
	return mTool->isInitialized();
}

QString ToolUsingIGSTK::getUid() const
{
	return Tool::mUid;
}

QString ToolUsingIGSTK::getName() const
{
	return Tool::mName;
}

double ToolUsingIGSTK::getTooltipOffset() const
{
	if(this->getProbe())
		return this->getProbe()->getProbeData().getDepthStart();
	return ToolImpl::getTooltipOffset();
//		return this->getTrackingService()->getTooltipOffset();
}

void ToolUsingIGSTK::setTooltipOffset(double val)
{
	if(this->getProbe())
		return;
	ToolImpl::setTooltipOffset(val);
//	this->getTrackingService()->setTooltipOffset(val);
}

bool ToolUsingIGSTK::isValid() const
{
	return mValid;
}

void ToolUsingIGSTK::createPolyData()
{
	QDir dir;
	if (!mTool->getInternalStructure().mGraphicsFileName.isEmpty()
					&& dir.exists(mTool->getInternalStructure().mGraphicsFileName))
	{
		vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
		reader->SetFileName(cstring_cast(mTool->getInternalStructure().mGraphicsFileName));
		reader->Update();
		mPolyData = reader->GetOutput();
	}
	else
	{
		vtkConeSourcePtr coneSource = vtkConeSourcePtr::New();
		coneSource->SetResolution(25);
		coneSource->SetRadius(10);
		coneSource->SetHeight(100);

		coneSource->SetDirection(0, 0, 1);
		double newCenter[3];
		coneSource->GetCenter(newCenter);
		newCenter[2] = newCenter[2] - coneSource->GetHeight() / 2;
		coneSource->SetCenter(newCenter);

		coneSource->Update();
		mPolyData = coneSource->GetOutput();
	}
}

bool ToolUsingIGSTK::isCalibrated() const
{
	Transform3D identity = Transform3D::Identity();
	Transform3D sMt = mTool->getInternalStructure().getCalibrationAsSSC();
	return !similar(sMt, identity);
}

Transform3D ToolUsingIGSTK::getCalibration_sMt() const
{
	Transform3D sMt = mTool->getInternalStructure().getCalibrationAsSSC();

	return sMt;
}

void ToolUsingIGSTK::setCalibration_sMt(Transform3D calibration)
{
	mTool->updateCalibration(calibration);
}

QString ToolUsingIGSTK::getCalibrationFileName() const
{
	return mTool->getInternalStructure().mCalibrationFilename;
}

TRACKING_SYSTEM ToolUsingIGSTK::getTrackerType()
{
	return mTool->getInternalStructure().mTrackerType;
}

void ToolUsingIGSTK::printInternalStructure()
{
	mTool->printInternalStructure();
}

ProbeDefinition ToolUsingIGSTK::getProbeSector() const
{
	if (mProbe)
		return mProbe->getProbeData();
	return ProbeDefinition();
}

std::map<int, Vector3D> ToolUsingIGSTK::getReferencePoints() const
{
	return mTool->getInternalStructure().mReferencePoints;
}

bool ToolUsingIGSTK::hasReferencePointWithId(int id)
{
  return this->getReferencePoints().count(id);
}

void ToolUsingIGSTK::addXml(QDomNode& dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();
	dataNode.toElement().setAttribute("uid", qstring_cast(this->getUid()));
	if (mProbe && mProbe->isValid())
	{
		QDomElement probeNode = doc.createElement("probe");
		mProbe->addXml(probeNode);
		dataNode.appendChild(probeNode);
	}
}

void ToolUsingIGSTK::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;
	if (mProbe)
	{
		QDomNode probeNode = dataNode.namedItem("probe");
		mProbe->parseXml(probeNode);
	}
}

void ToolUsingIGSTK::toolTransformAndTimestampSlot(Transform3D matrix, double timestamp)
{
	Transform3D prMt_filtered = matrix;

	if (mTrackingPositionFilter)
	{
		mTrackingPositionFilter->addPosition(matrix, timestamp);
		prMt_filtered = mTrackingPositionFilter->getFilteredPosition();
	}

	(*mPositionHistory)[timestamp] = matrix; // store original in history
	m_prMt = prMt_filtered;
	emit toolTransformAndTimestamp(m_prMt, timestamp);

//	ToolImpl::set_prMt(matrix, timestamp);
}

void ToolUsingIGSTK::calculateTpsSlot()
{
	int tpsNr = 0;

	int numberOfTransformsToCheck = ((mPositionHistory->size() >= 10) ? 10 : mPositionHistory->size());
	if (	numberOfTransformsToCheck <= 1)
	{
		emit tps(0);
		return;
	}

	TimedTransformMap::reverse_iterator it = mPositionHistory->rbegin();
	double lastTransform = it->first;
	for (int i = 0; i < numberOfTransformsToCheck; ++i)
		++it;
	double firstTransform = it->first;
	double secondsPassed = (lastTransform - firstTransform) / 1000;

	if (!similar(secondsPassed, 0))
		tpsNr = (int) (numberOfTransformsToCheck / secondsPassed);

	emit tps(tpsNr);
}

void ToolUsingIGSTK::toolVisibleSlot(bool on)
{
	if (on)
		mTpsTimer.start(1000); //calculate tps every 1 seconds
	else
		mTpsTimer.stop();
}

}//namespace cx
