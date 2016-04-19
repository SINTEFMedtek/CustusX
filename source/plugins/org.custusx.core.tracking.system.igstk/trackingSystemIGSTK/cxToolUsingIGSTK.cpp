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

#define _USE_MATH_DEFINES
#include "cxToolUsingIGSTK.h"

#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkSTLReader.h>
#include <QDir>
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
				mTool(igstkTool), mPolyData(NULL),
				mValid(false), mConfigured(false), mTracked(false)
{
	mTimestamp = 0;
	Tool::mUid = mTool->getInternalStructure().mUid;
	Tool::mName = mTool->getInternalStructure().mName;
    mValid = igstkTool->isValid();

	this->createPolyData();

	connect(mTool.get(), &IgstkTool::toolTransformAndTimestamp, this,
					&ToolUsingIGSTK::toolTransformAndTimestampSlot);
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
        mPolyData = Tool::createDefaultPolyDataCone();
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

void ToolUsingIGSTK::set_prMt(const Transform3D& prMt, double timestamp)
{

}

void ToolUsingIGSTK::setVisible(bool vis)
{

}


}//namespace cx
