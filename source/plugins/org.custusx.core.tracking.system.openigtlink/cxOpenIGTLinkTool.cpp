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

//#define _USE_MATH_DEFINES
#include "cxOpenIGTLinkTool.h"

#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkSTLReader.h>
#include <QDir>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>

#include "cxTypeConversions.h"
#include "cxProbeData.h"
#include "cxProbeImpl.h"
#include "cxTrackingPositionFilter.h"
#include "cxLogger.h"

namespace cx
{

OpenIGTLinkTool::OpenIGTLinkTool(QString uid) :
    ToolImpl(uid, uid),
    mPolyData(NULL),
    mValid(false),
    mConfigured(false),
    mTracked(false)
{
    mTimestamp = 0;
//    Tool::mUid = mTool->getInternalStructure().mUid;
//    Tool::mName = mTool->getInternalStructure().mName;
//    mValid = igstkTool->isValid();
    mValid = true;

    this->createPolyData();

    /*
    connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D, double)));
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
    */
}

OpenIGTLinkTool::~OpenIGTLinkTool()
{
}

std::set<Tool::Type> OpenIGTLinkTool::getTypes() const
{
    std::set<Type> retval;
    //TODO remove hardcoded type
    retval.insert(Tool::TOOL_POINTER);

//    if (mTool->getInternalStructure().mIsReference)
//        retval.insert(OpenIGTLinkTool::TOOL_REFERENCE);
//    if (mTool->getInternalStructure().mIsPointer)
//        retval.insert(OpenIGTLinkTool::TOOL_POINTER);
//    if (mTool->getInternalStructure().mIsProbe)
//        retval.insert(OpenIGTLinkTool::TOOL_US_PROBE);

    return retval;
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

int OpenIGTLinkTool::getIndex() const
{
    //TODO what is this??
    return 0;
}

double OpenIGTLinkTool::getTooltipOffset() const
{
    if(this->getProbe())
        return this->getProbe()->getProbeData().getDepthStart();
    return ToolImpl::getTooltipOffset();
}

void OpenIGTLinkTool::setTooltipOffset(double val)
{
    if(this->getProbe())
        return;
    ToolImpl::setTooltipOffset(val);
}

//bool OpenIGTLinkTool::isValid() const
//{
//    return mValid;
//}

void OpenIGTLinkTool::createPolyData()
{
//    QDir dir;
//    if (!mTool->getInternalStructure().mGraphicsFileName.isEmpty()
//                    && dir.exists(mTool->getInternalStructure().mGraphicsFileName))
//    {
//        vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
//        reader->SetFileName(cstring_cast(mTool->getInternalStructure().mGraphicsFileName));
//        reader->Update();
//        mPolyData = reader->GetOutput();
//    }
//    else
//    {
        //TODO this is copy paste from IGSTK tool: refactor out
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
//    }
}

bool OpenIGTLinkTool::isCalibrated() const
{
    //TODO how to know if an openigtlink tool is calibrated??
//    Transform3D identity = Transform3D::Identity();
//    Transform3D sMt = mTool->getInternalStructure().getCalibrationAsSSC();
//    return !similar(sMt, identity);
    return true;
}

Transform3D OpenIGTLinkTool::getCalibration_sMt() const
{
    //Transform3D sMt = mTool->getInternalStructure().getCalibrationAsSSC();
    Transform3D identity = Transform3D::Identity();
    return identity;
}

void OpenIGTLinkTool::setCalibration_sMt(Transform3D calibration)
{
    Q_UNUSED(calibration);
    //mTool->updateCalibration(calibration);
    CX_LOG_WARNING() << "Cannot set calibration on a openigtlink tool";
}

/*QString OpenIGTLinkTool::getCalibrationFileName() const
{
    return mTool->getInternalStructure().mCalibrationFilename;
}*/

//TRACKING_SYSTEM OpenIGTLinkTool::getTrackerType()
//{
//    return mTool->getInternalStructure().mTrackerType;
//}

void OpenIGTLinkTool::printInternalStructure()
{
    //TODO make a print self function
    CX_LOG_DEBUG() << "OpenIGTLinkTool is not able to print itself atm.";
    //mTool->printInternalStructure();
}

std::map<int, Vector3D> OpenIGTLinkTool::getReferencePoints() const
{
    std::map<int, Vector3D> retval;
//    return mTool->getInternalStructure().mReferencePoints;
    return retval;
}

bool OpenIGTLinkTool::hasReferencePointWithId(int id)
{
  return this->getReferencePoints().count(id);
}

//void OpenIGTLinkTool::addXml(QDomNode& dataNode)
//{
//    QDomDocument doc = dataNode.ownerDocument();
//    dataNode.toElement().setAttribute("uid", qstring_cast(this->getUid()));
//    if (mProbe && mProbe->isValid())
//    {
//        QDomElement probeNode = doc.createElement("probe");
//        mProbe->addXml(probeNode);
//        dataNode.appendChild(probeNode);
//    }
//}

//void OpenIGTLinkTool::parseXml(QDomNode& dataNode)
//{
//    if (dataNode.isNull())
//        return;
//    if (mProbe)
//    {
//        QDomNode probeNode = dataNode.namedItem("probe");
//        mProbe->parseXml(probeNode);
//    }
//}

void OpenIGTLinkTool::toolTransformAndTimestampSlot(Transform3D matrix, double timestamp)
{
    Transform3D prMt_filtered = matrix;

    if (mTrackingPositionFilter)
    {
        mTrackingPositionFilter->addPosition(matrix, timestamp);
        prMt_filtered = mTrackingPositionFilter->getFilteredPosition();
    }

    mTimestamp = timestamp;
    (*mPositionHistory)[timestamp] = matrix; // store original in history
    m_prMt = prMt_filtered;
    emit toolTransformAndTimestamp(m_prMt, timestamp);

//	ToolImpl::set_prMt(matrix, timestamp);
}

void OpenIGTLinkTool::calculateTpsSlot()
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

void OpenIGTLinkTool::toolVisibleSlot(bool on)
{
    if (on)
        mTpsTimer.start(1000); //calculate tps every 1 seconds
    else
        mTpsTimer.stop();
}

void OpenIGTLinkTool::set_prMt(const Transform3D& prMt, double timestamp)
{

}

void OpenIGTLinkTool::setVisible(bool vis)
{

}


}//namespace cx
