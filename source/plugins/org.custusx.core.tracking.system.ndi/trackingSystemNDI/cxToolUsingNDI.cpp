/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolUsingNDI.h"

#include <algorithm>
#include <iterator>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>

#include "cxNdiTool.h"
#include "cxProbeImpl.h"
#include "cxTrackingPositionFilter.h"
#include "cxTypeConversions.h"

namespace cx
{

ToolUsingNDI::ToolUsingNDI(NdiToolPtr ndiTool) :
	ToolImpl(""),
	mTool(ndiTool),
	mValid(false),
	mTimestamp(0)
{
	Tool::mUid = getToolFileToolStructure()->mUid;
	Tool::mName = getToolFileToolStructure()->mName;
	mValid = ndiTool->isValid();

	this->createToolGraphic();

	connect(mTool.get(), &NdiTool::toolTransformAndTimestamp,
			this, &ToolUsingNDI::toolTransformAndTimestampSlot);
	connect(mTool.get(), &NdiTool::toolVisible,
			this, &ToolUsingNDI::toolVisibleSlot);
	connect(mTool.get(), &NdiTool::toolVisible,
			this, &ToolUsingNDI::toolVisible);
	connect(mTool.get(), &NdiTool::positionWarning,
			this, &ToolUsingNDI::setPositionWarning);
	connect(&mTpsTimer, &QTimer::timeout, this, &ToolUsingNDI::calculateTpsSlot);

	if (getToolFileToolStructure()->mIsProbe)
	{
		mProbe = ProbeImpl::New(getToolFileToolStructure()->mInstrumentId,
								getToolFileToolStructure()->mInstrumentScannerId);
		connect(mProbe.get(), SIGNAL(sectorChanged()), this, SIGNAL(toolProbeSector()));
	}
}

ToolUsingNDI::~ToolUsingNDI()
{
}

ProbePtr ToolUsingNDI::getProbe() const
{
	return mProbe;
}

bool ToolUsingNDI::getVisible() const
{
	return mTool->isVisible();
}

bool ToolUsingNDI::isInitialized() const
{
	return mTool->isInitialized();
}

QString ToolUsingNDI::getUid() const
{
	return Tool::mUid;
}

QString ToolUsingNDI::getName() const
{
	return Tool::mName;
}

double ToolUsingNDI::getTooltipOffset() const
{
	if (this->getProbe() && !this->hasType(Tool::TOOL_SCOPE))
		return this->getProbe()->getProbeDefinition().getDepthStart();
	return ToolImpl::getTooltipOffset();
}

void ToolUsingNDI::setTooltipOffset(double val)
{
	if (this->getProbe() && !this->hasType(Tool::TOOL_SCOPE))
		return;
	ToolImpl::setTooltipOffset(val);
}

bool ToolUsingNDI::isValid() const
{
	return mValid;
}

bool ToolUsingNDI::isCalibrated() const
{
	Transform3D identity = Transform3D::Identity();
	Transform3D sMt = getToolFileToolStructure()->getCalibrationAsSSC();
	return !similar(sMt, identity);
}

Transform3D ToolUsingNDI::getCalibration_sMt() const
{
	return getToolFileToolStructure()->getCalibrationAsSSC();
}

void ToolUsingNDI::setCalibration_sMt(Transform3D calibration)
{
	getToolFileToolStructure()->setCalibration(calibration);
	getToolFileToolStructure()->saveCalibrationToFile();
}

ToolFileParser::ToolInternalStructurePtr ToolUsingNDI::getToolFileToolStructure() const
{
	return mTool->getInternalStructure();
}

void ToolUsingNDI::addXml(QDomNode& dataNode)
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

void ToolUsingNDI::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;
	if (mProbe)
	{
		QDomNode probeNode = dataNode.namedItem("probe");
		mProbe->parseXml(probeNode);
	}
}

void ToolUsingNDI::toolTransformAndTimestampSlot(Transform3D prMs, double timestamp, ToolPositionMetadata metadata)
{
	Transform3D prMt = prMs * getCalibration_sMt();
	Transform3D prMt_filtered = prMt;

	if (mTrackingPositionFilter)
	{
		mTrackingPositionFilter->addPosition(prMt, timestamp);
		prMt_filtered = mTrackingPositionFilter->getFilteredPosition();
	}

	mTimestamp = timestamp;
	mMetadata[timestamp] = metadata;

	if (this->getVisible())
		(*mPositionHistory)[timestamp] = prMt;

	m_prMt = prMt_filtered;
	emit toolTransformAndTimestamp(m_prMt, timestamp);
}

void ToolUsingNDI::calculateTpsSlot()
{
	emit tps(calculateTrackingRate(*mPositionHistory));
}

int ToolUsingNDI::calculateTrackingRate(const TimedTransformMap& history)
{
	// Rate over the last (up to) 10 positions: N positions span N-1 intervals.
	// (The version inherited from the IGSTK tool stepped one position too far,
	// past the start of the history when it had 10 or fewer positions.)
	int retval = 0;
	size_t numberOfPositions = std::min<size_t>(history.size(), 10);
	if (numberOfPositions >= 2)
	{
		TimedTransformMap::const_reverse_iterator last = history.rbegin();
		TimedTransformMap::const_reverse_iterator first = last;
		std::advance(first, numberOfPositions - 1);
		double secondsPassed = (last->first - first->first) / 1000.0;
		if (!similar(secondsPassed, 0))
		{
			retval = int((numberOfPositions - 1) / secondsPassed + 0.5);
		}
	}
	return retval;
}

void ToolUsingNDI::toolVisibleSlot(bool on)
{
	if (on)
		mTpsTimer.start(1000);
	else
		mTpsTimer.stop();
}

void ToolUsingNDI::set_prMt(const Transform3D& prMt, double timestamp)
{
	Q_UNUSED(prMt);
	Q_UNUSED(timestamp);
}

void ToolUsingNDI::setVisible(bool vis)
{
	Q_UNUSED(vis);
}

} // namespace cx
