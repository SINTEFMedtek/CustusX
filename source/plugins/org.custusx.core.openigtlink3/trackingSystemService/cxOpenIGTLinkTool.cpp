/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxOpenIGTLinkTool.h"

#include <vtkConeSource.h>
#include "cxTrackingPositionFilter.h"
#include "cxLogger.h"
#include "cxProbeImpl.h"

namespace cx
{

OpenIGTLinkTool::OpenIGTLinkTool(QString uid, igtlio::BaseConverter::EQUIPMENT_TYPE equipmentType) :
    ToolImpl(uid, uid),
    mPolyData(NULL),
    mTimestamp(0),
    m_sMt_calibration(Transform3D::Identity())
{
	CX_LOG_DEBUG() << "OpenIGTLinkTool equipmentType: " << equipmentType;
    connect(&mTpsTimer, SIGNAL(timeout()), this, SLOT(calculateTpsSlot()));

	mTypes = this->determineType(equipmentType);
    if (this->isProbe())
    {
		// See ProbeCalibsConfigs.xml
		// PlusDeviceSet_OpenIGTLinkCommandsTest - needs to be the same as <USScanner><Name>
		// ProbeToReference - needs to be the same as <USProbe><Name>
		mProbe = ProbeImpl::New("ProbeToReference", "PlusDeviceSet_OpenIGTLinkCommandsTest");
        connect(mProbe.get(), SIGNAL(sectorChanged()), this, SIGNAL(toolProbeSector()));
	}

    this->createPolyData();
    this->toolVisibleSlot(true);
}

OpenIGTLinkTool::~OpenIGTLinkTool()
{
}

std::set<Tool::Type> OpenIGTLinkTool::getTypes() const
{
    return mTypes;
}

vtkPolyDataPtr OpenIGTLinkTool::getGraphicsPolyData() const
{
    return mPolyData;
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
    //TODO add some logic, visible if transform arrived in the last X seconds???
    return true;
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

std::set<Tool::Type> OpenIGTLinkTool::determineType(const  igtlio::BaseConverter::EQUIPMENT_TYPE equipmentType) const
{
	std::set<Type> retval;
	retval.insert(TOOL_POINTER);
	if (equipmentType == igtlio::BaseConverter::US_PROBE || equipmentType == igtlio::BaseConverter::TRACKED_US_PROBE)
		retval.insert(TOOL_US_PROBE);

	return retval;
}

bool OpenIGTLinkTool::isProbe() const
{
    return (mTypes.find(TOOL_US_PROBE) != mTypes.end()) ? true : false;
}

void OpenIGTLinkTool::createPolyData()
{
    mPolyData = Tool::createDefaultPolyDataCone();
}

bool OpenIGTLinkTool::isCalibrated() const
{
    Transform3D identity = Transform3D::Identity();
    bool calibrated = !similar(m_sMt_calibration, identity);
    CX_LOG_DEBUG() << "Checking if openiglink tool is calibratated: " << calibrated;

    return calibrated;
}

Transform3D OpenIGTLinkTool::getCalibration_sMt() const
{
    return m_sMt_calibration;
}

void OpenIGTLinkTool::setCalibration_sMt(Transform3D sMt)
{
    if(!similar(m_sMt_calibration, sMt))
    {
        m_sMt_calibration = sMt;
        CX_LOG_INFO() << mName << " got an updated calibration";
    }
}

void OpenIGTLinkTool::toolTransformAndTimestampSlot(Transform3D prMs, double timestamp)
{
    mTimestamp = timestamp;// /1000000;
    Transform3D prMt = prMs * m_sMt_calibration;
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
    for (int i = 0; i < numberOfTransformsToCheck-1; ++i)
    {
        ++rit;
    }
    double firstTransform = rit->first;
    double secondsPassed = (lastTransform - firstTransform) / 1000;

    if (!similar(secondsPassed, 0))
        tpsNr = (int) (numberOfTransformsToCheck / secondsPassed);
    emit tps(tpsNr);
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
    CX_LOG_WARNING() << "Cannot set visible on a openigtlink tool.";
}

}//namespace cx
