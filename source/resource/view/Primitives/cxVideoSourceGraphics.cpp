/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxVideoSourceGraphics.h"

#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkImageMask.h>
#include <vtkImageThreshold.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTransformTextureCoords.h>
#include <vtkDataSetMapper.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkActor.h>
#include <vtkImageChangeInformation.h>
#include <vtkLookupTable.h>

#include "cxUltrasoundSectorSource.h"

#include "cxTool.h"
#include "cxRegistrationTransform.h"
#include "cxVideoGraphics.h"
#include "cxVideoSource.h"
#include "cxSpaceProvider.h"
#include "cxLogger.h"
#include "cxCoreServices.h"
#include "cxTrackingService.h"

namespace cx
{

VideoSourceGraphics::VideoSourceGraphics(CoreServicesPtr services, bool useMaskFilter)
{
    mServices = services;
    mClipToSector = true;
    mPipeline.reset(new VideoGraphics());
    mShowInToolSpace = true;
}

VideoSourceGraphics::~VideoSourceGraphics()
{
}

void VideoSourceGraphics::setShowInToolSpace(bool on)
{
    mShowInToolSpace = on;
}

vtkActorPtr VideoSourceGraphics::getActor()
{
    return mPipeline->getActor();
}

ToolPtr VideoSourceGraphics::getTool()
{
    return mTool;
}

ProbeSector VideoSourceGraphics::getProbeDefinition()
{
    return mProbeDefinition;
}

void VideoSourceGraphics::setTool(ToolPtr tool)
{
    if (tool==mTool)
        return;

    if (mTool)
    {
        disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
        disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
    }

    // accept only tool with a probe sector
    if (tool && tool->getProbe())
    {
        mTool = tool;
    }

    //double scopeFocusDepth = 30; //mm
    //if (tool && tool->hasType(Tool::TOOL_SCOPE))
    //    mTool->setTooltipOffset(scopeFocusDepth);

    // setup new
    if (mTool )
    {
        connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
        connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
    }

    this->probeSectorChanged();
}

void VideoSourceGraphics::setClipToSector(bool on)
{
    mClipToSector = on;
    this->probeSectorChanged();
}

void VideoSourceGraphics::probeSectorChanged()
{
    if (!mTool || !mTool->getProbe())
        return;

    mProbeDefinition.setData(mTool->getProbe()->getProbeDefinition());
    if (mClipToSector)
    {
        mPipeline->setClip(mProbeDefinition.getSector());
    }
    else
    {
        mPipeline->setClip(NULL);
    }
    this->receiveTransforms(mTool->get_prMt(), 0);

    mPipeline->update();
}


void VideoSourceGraphics::setRealtimeStream(VideoSourcePtr data)
{
    //Don't do anything if data is unchanged
    if (mData == data)
        return;
    if (mData)
    {
        disconnect(mData.get(), &VideoSource::newFrame, this, &VideoSourceGraphics::newDataSlot);
        mPipeline->setInputVideo(NULL);
    }

    mData = data;

    if (mData)
    {
        connect(mData.get(), &VideoSource::newFrame, this, &VideoSourceGraphics::newDataSlot);
        mPipeline->setInputVideo(mData->getVtkImageData());
    }

    this->newDataSlot();
}

void VideoSourceGraphics::receiveTransforms(Transform3D prMt, double timestamp)
{
    if (!mShowInToolSpace)
        return;
    Transform3D rMpr = mServices->spaceProvider()->get_rMpr();
    Transform3D tMu = mProbeDefinition.get_tMu();

    if (mTool->hasType(Tool::TOOL_SCOPE))
    {
        this->updateBronchoscopyTool();
        Transform3D Rx = createTransformRotateX(M_PI);
        Transform3D Rz = createTransformRotateZ(M_PI/2.0);
        Transform3D R = (Rx * Rz);
        if (mData)
        {
            QString streamUid = mData->getUid();
            ProbeDefinition probeDefinition = mTool->getProbe()->getProbeDefinition(streamUid);
            Vector3D origin_p = probeDefinition.getOrigin_p();
            Vector3D spacing = probeDefinition.getSpacing();
            QSize size = probeDefinition.getSize();
            //double scpoeFocusDepth = mTool->getTooltipOffset();
            double videoFocusDepth = 30;
            //TO DO: Scale spacing based on focus depth?
            Vector3D origin_u(origin_p[0]*spacing[0], (origin_p[1] + size.height()/2)*spacing[1], origin_p[2]*spacing[2] + videoFocusDepth);
            Transform3D T = createTransformTranslate(-origin_u);
            tMu = R * T;
        }
    }

    Transform3D rMu = rMpr * prMt * tMu;
    mPipeline->setActorUserMatrix(rMu.getVtkMatrix());
}

void VideoSourceGraphics::updateBronchoscopyTool()
{
    std::map<QString, ToolPtr> tools = mServices->tracking()->getTools();
    if(isBronchoscopyTool(mTool))
    {
        if(!isToolInToolMap(tools, mTool))
        {
            if(mOriginalTool)
                setTool(mOriginalTool); //Return to original tool if bronchoscopy tool is disabled
        }
        return;
    }

    for (std::map<QString, ToolPtr>::const_iterator iter = tools.begin(); iter != tools.end(); ++iter)
    {
        ToolPtr bronchoscopyTool = iter->second;
        if(isBronchoscopyTool(bronchoscopyTool))
            if(mTool->getUid() == bronchoscopyTool->getUid()) //Assuming tool uids are equal. See cxBronchoscopyTool
            {
                mOriginalTool = mTool;
                setTool(bronchoscopyTool);
            }
    }
}

bool  VideoSourceGraphics::isToolInToolMap(std::map<QString, ToolPtr> tools, ToolPtr tool)
{
    for (std::map<QString, ToolPtr>::const_iterator iter = tools.begin(); iter != tools.end(); ++iter)
    {
        if (tool == iter->second)
            return true;
    }
    return false;
}

bool VideoSourceGraphics::isBronchoscopyTool(ToolPtr tool)
{
    QString toolName = tool->getName();
    if(toolName.contains("Bronchoscopy Navigation", Qt::CaseInsensitive))
        return true;
    return false;
}

void VideoSourceGraphics::newDataSlot()
{
    if (!mData || !mData->validData())
    {
        mPipeline->setVisibility(false);
        emit newData();
        return;
    }

    mPipeline->update();

    bool visible = mData->validData();
    if (mShowInToolSpace)
        visible = visible && mTool && mTool->getVisible();
    mPipeline->setVisibility(visible);

    emit newData();
}

} // namespace cx
