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

#include "cxOpenIGTLinkTool.h"

#include <QDateTime>

#include "cxTrackingPositionFilter.h"
#include "cxLogger.h"
#include "cxProbeImpl.h"

namespace cx
{
OpenIGTLinkTool::OpenIGTLinkTool(ConfigurationFileParser::ToolStructure configFileToolStructure, ToolFileParser::ToolInternalStructurePtr toolFileToolStructure) :
	ToolImpl(toolFileToolStructure->mUid, toolFileToolStructure->mUid),
	mTimestamp(0),
	mVisible(false),
	mLastReceivedPositionTime(0),
	mConfigFileToolStructure(configFileToolStructure),
	mToolFileToolStructure(toolFileToolStructure)

{
//	CX_LOG_DEBUG() << "OpenIGTLinkTool constr mInstrumentId: " << mToolFileToolStructure.mInstrumentId << " mInstrumentScannerId: " << mToolFileToolStructure.mInstrumentScannerId;
//	CX_LOG_DEBUG() << "OpenIGTLinkTool constr mOpenIGTLinkTransformId: " << mConfigFileToolStructure.mOpenIGTLinkTransformId << " mOpenIGTLinkImageId: " << mConfigFileToolStructure.mOpenIGTLinkImageId;
	connect(&mTpsTimer, &QTimer::timeout, this, &OpenIGTLinkTool::calculateTpsSlot);
	connect(&mTpsTimer, &QTimer::timeout, this, &OpenIGTLinkTool::calculateVisible);//Use tps timer to calculate visibility

	if(toolFileToolStructure->mIsProbe)
	{
//		CX_LOG_DEBUG() << "OpenIGTLinkTool is probe mInstrumentId: " << mToolFileToolStructure.mInstrumentId << " mInstrumentScannerId: " << mToolFileToolStructure.mInstrumentScannerId;
//		CX_LOG_DEBUG() << "OpenIGTLinkTool is probe";
//		mProbe = ProbeImpl::New(mConfigFileToolStructure.mOpenIGTLinkTransformId, mConfigFileToolStructure.mOpenIGTLinkImageId);
//		mProbe = ProbeImpl::New(mToolFileToolStructure.mInstrumentId, mToolFileToolStructure.mInstrumentScannerId);
		// See ProbeCalibsConfigs.xml
		// PlusDeviceSet_OpenIGTLinkCommandsTest - needs to be the same as <USScanner><Name>
		// ProbeToReference - needs to be the same as <USProbe><Name>
		mProbe = ProbeImpl::New("ProbeToReference", "PlusDeviceSet_OpenIGTLinkCommandsTest");
		connect(mProbe.get(), SIGNAL(sectorChanged()), this, SIGNAL(toolProbeSector()));
	}

	this->createToolGraphic();
	this->toolVisibleSlot(true);
}

bool OpenIGTLinkTool::isThisTool(QString OpenIGTLinkId)
{
	bool retval = false;
	if(OpenIGTLinkId.compare(this->mConfigFileToolStructure.mOpenIGTLinkTransformId, Qt::CaseInsensitive) == 0)
		retval = true;
	else if(OpenIGTLinkId.compare(this->mConfigFileToolStructure.mOpenIGTLinkImageId, Qt::CaseInsensitive) == 0)
		retval = true;
	return retval;
}

OpenIGTLinkTool::~OpenIGTLinkTool()
{
}

ProbePtr OpenIGTLinkTool::getProbe() const
{
    return mProbe;
}

double OpenIGTLinkTool::getTimestamp() const
{
    return mTimestamp;
}

bool OpenIGTLinkTool::getVisible() const
{
	return mVisible;
}

bool OpenIGTLinkTool::isInitialized() const
{
    //TODO when is a tool initialized? when it is connected to the tracker?
    return true;
}

QString OpenIGTLinkTool::getUid() const
{
    return Tool::mUid;
}

QString OpenIGTLinkTool::getName() const
{
    return Tool::mName;
}

double OpenIGTLinkTool::getTooltipOffset() const
{
    if(this->getProbe())
        return this->getProbe()->getProbeDefinition().getDepthStart();
    return ToolImpl::getTooltipOffset();
}

void OpenIGTLinkTool::setTooltipOffset(double val)
{
    if(this->getProbe())
        return;
    ToolImpl::setTooltipOffset(val);
}

bool OpenIGTLinkTool::isCalibrated() const
{
    Transform3D identity = Transform3D::Identity();

		Transform3D sMt = this->getCalibration_sMt();
		bool calibrated = !similar(sMt, identity);
		CX_LOG_DEBUG() << "Checking if openiglink tool is calibratated: " << calibrated;
		return calibrated;
}

Transform3D OpenIGTLinkTool::getCalibration_sMt() const
{
	return mToolFileToolStructure->getCalibrationAsSSC();
}

void OpenIGTLinkTool::setCalibration_sMt(Transform3D sMt)
{
	CX_LOG_INFO() << mName << " got an updated calibration";
	CX_LOG_WARNING() << "OpenIGTLinkTool::setCalibration_sMt() Receiving calibration. Should file be updated, or should it be discarded and use calibration form file instead?";
	CX_LOG_WARNING() << "Current implementation discards this received calibration. sMt: " << sMt;
//	mToolFileToolStructure.mCalibration = sMt;
	//write to file
//	mInternalStructure.saveCalibrationToFile();
}

void OpenIGTLinkTool::toolTransformAndTimestampSlot(Transform3D prMs, double timestamp)
{
//    mTimestamp = timestamp;// /1000000;
	mTimestamp = timestamp * 1000;
	mLastReceivedPositionTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	double diff = mLastReceivedPositionTime - mTimestamp;

	if(diff > 1000)
		CX_LOG_WARNING() << "Difference between system time and received tool timestamp: " << diff;

	//TODO: Make sure this is the way we want to handle this
    //Current implementation get all transforms between tool frame and tracking system.
    //Another solution is to get transforms between tool frame and ref frame.

    Transform3D prMt = prMs * this->getCalibration_sMt();
    Transform3D prMt_filtered = prMt;

    if (mTrackingPositionFilter)
    {
        mTrackingPositionFilter->addPosition(prMt, mTimestamp);
        prMt_filtered = mTrackingPositionFilter->getFilteredPosition();
    }

    (*mPositionHistory)[mTimestamp] = prMt; // store original in history
    m_prMt = prMt_filtered;
    emit toolTransformAndTimestamp(m_prMt, mTimestamp);
}

void OpenIGTLinkTool::calculateTpsSlot()
{
    int tpsNr = 0;
    size_t numberOfTransformsToCheck = ((mPositionHistory->size() >= 10) ? 10 : mPositionHistory->size());
    if (numberOfTransformsToCheck <= 1)
    {
        emit tps(0);
        return;
    }

    TimedTransformMap::reverse_iterator rit = mPositionHistory->rbegin();
    double lastTransform = rit->first;
		for (size_t i = 0; i < numberOfTransformsToCheck-1; ++i)
    {
        ++rit;
    }
    double firstTransform = rit->first;
    double secondsPassed = (lastTransform - firstTransform) / 1000;

    if (!similar(secondsPassed, 0))
				tpsNr = int(numberOfTransformsToCheck / secondsPassed);
    emit tps(tpsNr);
}

void OpenIGTLinkTool::calculateVisible()
{
	//Compare only timestamps from this computer, and not the received timestamps
	//(as this computer man not be in sync with the one creating the messages)
	qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	double diff = currentTime - mLastReceivedPositionTime;
//	CX_LOG_DEBUG() << "diff: " << diff;
	if(diff > 500)//Set visible to false if last position is more than 500 ms old
		setVisible(false);
	else
		setVisible(true);
}

void OpenIGTLinkTool::toolVisibleSlot(bool on)
{
    if (on)
        mTpsTimer.start(1000); //calculate tps every 1 seconds
    else
        mTpsTimer.stop();
}

void OpenIGTLinkTool::setVisible(bool vis)
{
	mVisible = vis;
	emit toolVisible(vis);
}

ToolFileParser::ToolInternalStructurePtr OpenIGTLinkTool::getToolFileToolStructure() const
{
	return mToolFileToolStructure;
}

bool OpenIGTLinkTool::isReference()
{
	return getToolFileToolStructure()->mIsReference;
}


}//namespace cx
