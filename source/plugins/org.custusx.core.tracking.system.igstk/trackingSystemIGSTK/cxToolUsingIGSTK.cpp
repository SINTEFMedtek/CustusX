/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#define _USE_MATH_DEFINES
#include "cxToolUsingIGSTK.h"

#include <QDateTime>
#include <QStringList>
#include <QTextStream>

#include "cxTypeConversions.h"
#include "cxProbeDefinition.h"
#include "cxProbeImpl.h"
#include "cxIgstkTool.h"
#include "cxTrackingPositionFilter.h"

namespace cx
{

ToolUsingIGSTK::ToolUsingIGSTK(IgstkToolPtr igstkTool) :
	ToolImpl(""),
				mTool(igstkTool),
				mValid(false), mConfigured(false), mTracked(false)
{
	mTimestamp = 0;
	Tool::mUid = getToolFileToolStructure()->mUid;
	Tool::mName = getToolFileToolStructure()->mName;
	mValid = igstkTool->isValid();

	this->createToolGraphic();

	connect(mTool.get(), &IgstkTool::toolTransformAndTimestamp, this,
					&ToolUsingIGSTK::toolTransformAndTimestampSlot);
	connect(mTool.get(), SIGNAL(attachedToTracker(bool)), this, SIGNAL(attachedToTracker(bool)));
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
	connect(&mTpsTimer, SIGNAL(timeout()), this, SLOT(calculateTpsSlot()));

	if (getToolFileToolStructure()->mIsProbe)
	{
		mProbe = ProbeImpl::New(getToolFileToolStructure()->mInstrumentId,
						getToolFileToolStructure()->mInstrumentScannerId);
		connect(mProbe.get(), SIGNAL(sectorChanged()), this, SIGNAL(toolProbeSector()));
	}
}

ToolUsingIGSTK::~ToolUsingIGSTK()
{
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
		return this->getProbe()->getProbeDefinition().getDepthStart();
	return ToolImpl::getTooltipOffset();
}

void ToolUsingIGSTK::setTooltipOffset(double val)
{
	if(this->getProbe())
		return;
	ToolImpl::setTooltipOffset(val);
}

bool ToolUsingIGSTK::isValid() const
{
	return mValid;
}

bool ToolUsingIGSTK::isCalibrated() const
{
	Transform3D identity = Transform3D::Identity();
	Transform3D sMt = getToolFileToolStructure()->getCalibrationAsSSC();
	return !similar(sMt, identity);
}

Transform3D ToolUsingIGSTK::getCalibration_sMt() const
{
	Transform3D sMt = getToolFileToolStructure()->getCalibrationAsSSC();

	return sMt;
}

void ToolUsingIGSTK::setCalibration_sMt(Transform3D calibration)
{
	mTool->updateCalibration(calibration);
}

QString ToolUsingIGSTK::getCalibrationFileName() const
{
	return getToolFileToolStructure()->mCalibrationFilename;
}

TRACKING_SYSTEM ToolUsingIGSTK::getTrackerType()
{
	return getToolFileToolStructure()->mTrackerType;
}

void ToolUsingIGSTK::printInternalStructure()
{
	mTool->printInternalStructure();
}

ToolFileParser::ToolInternalStructurePtr ToolUsingIGSTK::getToolFileToolStructure() const
{
	return mTool->getInternalStructure();
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

void ToolUsingIGSTK::toolTransformAndTimestampSlot(Transform3D matrix, double timestamp, ToolPositionMetadata metadata)
{
	Transform3D prMt_filtered = matrix;

	if (mTrackingPositionFilter)
	{
		mTrackingPositionFilter->addPosition(matrix, timestamp);
		prMt_filtered = mTrackingPositionFilter->getFilteredPosition();
	}

	mTimestamp = timestamp;
	mMetadata[timestamp] = metadata;

	// Store positions in history, but only if visible - the history has no concept of visibility
	if (this->getVisible())
		(*mPositionHistory)[timestamp] = matrix;
	m_prMt = prMt_filtered;
	emit toolTransformAndTimestamp(m_prMt, timestamp);

//	ToolImpl::set_prMt(matrix, timestamp);
}

void ToolUsingIGSTK::calculateTpsSlot()
{
	int tpsNr = 0;

	size_t numberOfTransformsToCheck = ((mPositionHistory->size() >= 10) ? 10 : mPositionHistory->size());
	if (	numberOfTransformsToCheck <= 1)
	{
		emit tps(0);
		return;
	}

	TimedTransformMap::reverse_iterator it = mPositionHistory->rbegin();
	double lastTransform = it->first;
	for (size_t i = 0; i < numberOfTransformsToCheck; ++i)
		++it;
	double firstTransform = it->first;
	double secondsPassed = (lastTransform - firstTransform) / 1000;

	if (!similar(secondsPassed, 0))
		tpsNr = int(numberOfTransformsToCheck / secondsPassed);

	emit tps(tpsNr);
}

void ToolUsingIGSTK::toolVisibleSlot(bool on)
{
	if (on)
		mTpsTimer.start(1000); //calculate tps every 1 seconds
	else
		mTpsTimer.stop();
}

void ToolUsingIGSTK::set_prMt(const Transform3D& prMt, double timestamp)
{
	Q_UNUSED(prMt);
	Q_UNUSED(timestamp);
}

void ToolUsingIGSTK::setVisible(bool vis)
{
	Q_UNUSED(vis);
}


}//namespace cx
